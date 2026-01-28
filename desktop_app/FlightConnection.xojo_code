#tag Class
Protected Class FlightConnection
	#tag Enum, Name = ConnectionMode, Flags = &h0
		Serial
		  WiFi
	#tag EndEnum

	#tag Method, Flags = &h0
		Function Connect(inPortName As String) As Boolean
		  // Connect to gateway via USB Serial
		  pConnectionMode = ConnectionMode.Serial
		  If pSerialConnection = Nil Then
		    LogMessage("SerialConnection is nil, cannot connect", "ERROR")
		    Return False
		  End If

		  // Find the serial device by name
		  Var theDevice As SerialDevice
		  For i As Integer = 0 To SerialDevice.Count - 1
		    If SerialDevice.At(i).Name = inPortName Then
		      theDevice = SerialDevice.At(i)
		      Exit
		    End If
		  Next

		  If theDevice = Nil Then
		    LogMessage("Serial device not found: " + inPortName, "ERROR")
		    Return False
		  End If

		  LogMessage("Connecting to gateway via USB: " + inPortName, "INFO")

		  // Configure and connect
		  Try
		    pSerialConnection.Device = theDevice
		    pSerialConnection.DataTerminalReady = True
		    pSerialConnection.Connect
		    pSerialIsOpen = True

		    // Clear buffers
		    pJsonAccumulator = ""
		    pJsonBraceDepth = 0
		    pReceiveBuffer = ""

		    LogMessage("USB Serial connected to " + inPortName, "INFO")
		    RaiseEvent ConnectionChanged(True)

		    Return True

		  Catch theError As IOException
		    LogMessage("Failed to open serial port: " + theError.Message, "ERROR")
		    RaiseEvent ErrorReceived("SERIAL_ERROR", "Failed to open serial port: " + theError.Message)
		    Return False
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function ConnectWifi(inHost As String, inPort As Integer) As Boolean
		  // Connect to gateway via TCP/WiFi
		  pConnectionMode = ConnectionMode.WiFi

		  If pTcpSocket = Nil Then
		    LogMessage("TCPSocket is nil, cannot connect", "ERROR")
		    Return False
		  End If

		  // Disconnect any existing connection
		  If pTcpIsConnected Then
		    pTcpSocket.Close
		    pTcpIsConnected = False
		  End If
		  If pSerialIsOpen Then
		    pSerialConnection.Close
		    pSerialIsOpen = False
		  End If

		  LogMessage("Connecting to gateway via WiFi: " + inHost + ":" + Str(inPort), "INFO")

		  // Clear buffers
		  pJsonAccumulator = ""
		  pJsonBraceDepth = 0
		  pReceiveBuffer = ""

		  // Connect
		  Try
		    pTcpSocket.Address = inHost
		    pTcpSocket.Port = inPort
		    pTcpSocket.Connect
		    Return True  // Actual connection confirmed in Connected event
		  Catch theError As RuntimeException
		    LogMessage("Failed to connect: " + theError.Message, "ERROR")
		    RaiseEvent ErrorReceived("TCP_ERROR", "Failed to connect: " + theError.Message)
		    Return False
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize connection handler

		  // Serial connection
		  pSerialConnection = New SerialConnection
		  pSerialConnection.Baud = SerialConnection.Baud115200
		  pSerialConnection.Bits = SerialConnection.Bits8
		  pSerialConnection.Parity = SerialConnection.Parities.None
		  pSerialConnection.StopBit = SerialConnection.StopBits.One
		  AddHandler pSerialConnection.DataReceived, AddressOf HandleSerialDataReceived
		  AddHandler pSerialConnection.Error, AddressOf HandleSerialConnectionError

		  // TCP socket for WiFi connection
		  pTcpSocket = New TCPSocket
		  AddHandler pTcpSocket.Connected, AddressOf HandleTcpConnected
		  AddHandler pTcpSocket.DataAvailable, AddressOf HandleTcpDataAvailable
		  AddHandler pTcpSocket.Error, AddressOf HandleTcpError

		  pConnectionMode = ConnectionMode.Serial

		  LogMessage("FlightConnection initialized", "DEBUG")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Destructor()
		  // Clean up resources
		  If pSerialConnection <> Nil Then
		    RemoveHandler pSerialConnection.DataReceived, AddressOf HandleSerialDataReceived
		    RemoveHandler pSerialConnection.Error, AddressOf HandleSerialConnectionError
		    pSerialConnection.Close
		    pSerialIsOpen = False
		  End If

		  If pTcpSocket <> Nil Then
		    RemoveHandler pTcpSocket.Connected, AddressOf HandleTcpConnected
		    RemoveHandler pTcpSocket.DataAvailable, AddressOf HandleTcpDataAvailable
		    RemoveHandler pTcpSocket.Error, AddressOf HandleTcpError
		    If pTcpIsConnected Then
		      pTcpSocket.Close
		    End If
		    pTcpIsConnected = False
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Disconnect()
		  // Close connection
		  LogMessage("Disconnecting", "INFO")

		  // Close serial if open
		  If pSerialIsOpen And pSerialConnection <> Nil Then
		    pSerialConnection.Close
		  End If
		  pSerialIsOpen = False

		  // Close TCP if connected
		  If pTcpIsConnected And pTcpSocket <> Nil Then
		    pTcpSocket.Close
		  End If
		  pTcpIsConnected = False

		  pReceiveBuffer = ""
		  pJsonAccumulator = ""
		  pJsonBraceDepth = 0
		  RaiseEvent ConnectionChanged(False)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Function GetSerialPorts() As String()
		  // Return array of available serial port names
		  Var thePorts() As String

		  For i As Integer = 0 To SerialDevice.Count - 1
		    Var theDevice As SerialDevice = SerialDevice.At(i)
		    Var theName As String = theDevice.Name

		    #If TargetMacOS Then
		      If theName.Contains("usbmodem") Then
		        thePorts.Add(theName)
		      End If
		    #ElseIf TargetWindows Then
		      If theName.BeginsWith("COM") Then
		        thePorts.Add(theName)
		      End If
		    #ElseIf TargetLinux Then
		      If theName.Contains("ttyACM") Or theName.Contains("ttyUSB") Then
		        thePorts.Add(theName)
		      End If
		    #Else
		      thePorts.Add(theName)
		    #EndIf
		  Next

		  Return thePorts
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleSerialDataReceived(inSender As SerialConnection)
		  // Read available data
		  Var theData As String = inSender.ReadAll(Encodings.UTF8)

		  // Sanitize data - remove null bytes
		  Var theSanitized As String = ""
		  For i As Integer = 0 To theData.Bytes - 1
		    Var theByte As Integer = theData.Middle(i, 1).Asc
		    If theByte >= 32 And theByte <= 126 Then
		      theSanitized = theSanitized + Chr(theByte)
		    ElseIf theByte = 9 Or theByte = 10 Or theByte = 13 Then
		      theSanitized = theSanitized + Chr(theByte)
		    End If
		  Next

		  If theSanitized.Length > 0 Then
		    LogData("RX", theSanitized)
		    pReceiveBuffer = pReceiveBuffer + theSanitized
		    ProcessReceivedData()
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleSerialConnectionError(inSender As SerialConnection, inError As RuntimeException)
		  #Pragma Unused inSender
		  If Not pSerialIsOpen Then
		    Return
		  End If

		  Var theErrorMsg As String = "USB Serial error"
		  If inError <> Nil Then
		    theErrorMsg = theErrorMsg + ": " + inError.Message
		  End If

		  LogMessage(theErrorMsg, "ERROR")
		  pSerialIsOpen = False

		  If pSerialConnection <> Nil Then
		    Try
		      pSerialConnection.Close
		    Catch e As RuntimeException
		      // Ignore close errors
		    End Try
		  End If

		  RaiseEvent ConnectionChanged(False)
		  RaiseEvent ErrorReceived("SERIAL_ERROR", theErrorMsg)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleTcpConnected(inSender As TCPSocket)
		  #Pragma Unused inSender
		  pTcpIsConnected = True
		  LogMessage("TCP connected to gateway", "INFO")
		  RaiseEvent ConnectionChanged(True)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleTcpDataAvailable(inSender As TCPSocket)
		  // Read available data from TCP socket
		  Var theData As String = inSender.ReadAll(Encodings.UTF8)

		  // Sanitize data - remove null bytes
		  Var theSanitized As String = ""
		  For i As Integer = 0 To theData.Bytes - 1
		    Var theByte As Integer = theData.Middle(i, 1).Asc
		    If theByte >= 32 And theByte <= 126 Then
		      theSanitized = theSanitized + Chr(theByte)
		    ElseIf theByte = 9 Or theByte = 10 Or theByte = 13 Then
		      theSanitized = theSanitized + Chr(theByte)
		    End If
		  Next

		  If theSanitized.Length > 0 Then
		    LogData("RX", theSanitized)
		    pReceiveBuffer = pReceiveBuffer + theSanitized
		    ProcessReceivedData()
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleTcpError(inSender As TCPSocket, theError As RuntimeException)
		  #Pragma Unused inSender

		  Var theErrorMsg As String = "TCP connection error"
		  If theError <> Nil Then
		    theErrorMsg = theErrorMsg + ": " + theError.Message
		  End If

		  LogMessage(theErrorMsg, "ERROR")

		  If pTcpIsConnected Then
		    pTcpIsConnected = False
		    If pTcpSocket <> Nil Then
		      Try
		        pTcpSocket.Close
		      Catch e As RuntimeException
		        // Ignore close errors
		      End Try
		    End If
		    RaiseEvent ConnectionChanged(False)
		  End If

		  RaiseEvent ErrorReceived("TCP_ERROR", theErrorMsg)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function IsConnected() As Boolean
		  // Check if connected via either serial or TCP
		  Return pSerialIsOpen Or pTcpIsConnected
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetConnectionMode() As ConnectionMode
		  Return pConnectionMode
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub LogData(inDirection As String, inData As String)
		  // Log raw data (for debugging)
		  RaiseEvent DebugLog(inDirection + ": " + inData)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub LogMessage(inMessage As String, inLevel As String = "INFO")
		  RaiseEvent DebugLog("[" + inLevel + "] " + inMessage)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ParseMessage(inJson As String)
		  // Parse a complete JSON message

		  // Notify listeners of raw message
		  RaiseEvent RawMessageReceived(inJson)

		  Try
		    Var theJson As New JSONItem(inJson)

		    If Not theJson.HasKey("type") Then
		      Return
		    End If

		    Var theType As String = theJson.Value("type")

		    Select Case theType

		    Case "tel"
		      // Telemetry packet from flight computer
		      Var theSample As New TelemetrySample
		      theSample.pTimeMs = theJson.Lookup("t", 0)
		      theSample.pAltitudeM = theJson.Lookup("alt", 0.0)
		      theSample.pDifferentialAltitudeM = theJson.Lookup("dalt", 0.0)
		      theSample.pVelocityMps = theJson.Lookup("vel", 0.0)
		      theSample.pPressurePa = theJson.Lookup("pres", 0.0)
		      theSample.pGroundPressurePa = theJson.Lookup("gpres", 0.0)
		      theSample.pGroundAltitudeM = theJson.Lookup("galt", 0.0)
		      theSample.pTemperatureC = theJson.Lookup("temp", 0.0)
		      theSample.pState = theJson.Lookup("state", "")
		      theSample.pFlags = theJson.Lookup("flags", 0)
		      theSample.pRssi = theJson.Lookup("rssi", 0)
		      theSample.pSnr = theJson.Lookup("snr", 0)

		      // GPS data
		      theSample.pGpsLatitude = theJson.Lookup("lat", 0.0)
		      theSample.pGpsLongitude = theJson.Lookup("lon", 0.0)
		      theSample.pGpsSpeedMps = theJson.Lookup("gspd", 0.0)
		      theSample.pGpsHeadingDeg = theJson.Lookup("hdg", 0.0)
		      theSample.pGpsSatellites = theJson.Lookup("sat", 0)
		      theSample.pGpsFix = theJson.Lookup("gps", False)

		      // Accelerometer data - gateway sends in g's (floats), convert to milli-g for storage
		      Var theAccelXg As Double = theJson.Lookup("ax", 0.0)
		      Var theAccelYg As Double = theJson.Lookup("ay", 0.0)
		      Var theAccelZg As Double = theJson.Lookup("az", 0.0)
		      theSample.pAccelX = Round(theAccelXg * 1000.0)  // Store as milli-g (Integer)
		      theSample.pAccelY = Round(theAccelYg * 1000.0)
		      theSample.pAccelZ = Round(theAccelZg * 1000.0)

		      // Gyroscope data - gateway sends in dps (floats), store directly
		      theSample.pGyroX = theJson.Lookup("gx", 0.0)  // Store as dps (Double)
		      theSample.pGyroY = theJson.Lookup("gy", 0.0)
		      theSample.pGyroZ = theJson.Lookup("gz", 0.0)

		      // Magnetometer data (in milligauss, integers)
		      theSample.pMagX = theJson.Lookup("mx", 0)
		      theSample.pMagY = theJson.Lookup("my", 0)
		      theSample.pMagZ = theJson.Lookup("mz", 0)

		      // Calculate pitch and roll from accelerometer (in degrees)
		      // IMU coordinate system: Y=up (vertical), X=right, Z=back (toward user)
		      // theAccelXg/Yg/Zg are already in g's from above

		      // Pitch = forward/back tilt (rotation around X-axis)
		      // Positive pitch = nose up, negative = nose down
		      theSample.pPitch = ATan2(theAccelZg, theAccelYg) * 180.0 / 3.14159

		      // Roll = left/right tilt (rotation around Z-axis)
		      // Positive roll = tilting right
		      theSample.pRoll = ATan2(-theAccelXg, theAccelYg) * 180.0 / 3.14159

		      // Calculate heading from magnetometer
		      // For horizontal plane heading, use X and Z (since Y is vertical)
		      theSample.pHeading = ATan2(theSample.pMagX, -theSample.pMagZ) * 180.0 / 3.14159
		      If theSample.pHeading < 0 Then
		        theSample.pHeading = theSample.pHeading + 360.0
		      End If

		      // Gateway GPS data
		      theSample.pGatewayGpsFix = theJson.Lookup("gw_gps", False)
		      theSample.pGatewayLatitude = theJson.Lookup("gw_lat", 0.0)
		      theSample.pGatewayLongitude = theJson.Lookup("gw_lon", 0.0)

		      RaiseEvent TelemetryReceived(theSample)

		    Case "link"
		      // Link status from gateway
		      // Note: "usb_connected" means USB cable connected, not LoRa link
		      // Only raise event for actual LoRa link status changes
		      Var theStatus As String = theJson.Lookup("status", "")
		      If theStatus = "connected" Then
		        RaiseEvent LinkStatusChanged(True)
		      ElseIf theStatus = "lost" Then
		        RaiseEvent LinkStatusChanged(False)
		      End If
		      // Ignore "usb_connected" - this is USB cable status, not LoRa link

		    Case "status"
		      // Gateway status response
		      Var theConnected As Boolean = theJson.Lookup("connected", False)
		      Var theRx As Integer = theJson.Lookup("rx", 0)
		      Var theTx As Integer = theJson.Lookup("tx", 0)
		      Var theRssi As Integer = theJson.Lookup("rssi", 0)
		      Var theSnr As Integer = theJson.Lookup("snr", 0)
		      RaiseEvent GatewayStatusReceived(theConnected, theRx, theTx, theRssi, theSnr)

		    Case "ack"
		      // Command acknowledgment
		      Var theId As Integer = theJson.Lookup("id", 0)
		      Var theOk As Boolean = theJson.Lookup("ok", False)
		      RaiseEvent AckReceived(theId, theOk)

		    Case "error"
		      Var theCode As String = theJson.Lookup("code", "")
		      Var theMessage As String = theJson.Lookup("message", "")
		      RaiseEvent ErrorReceived(theCode, theMessage)

		    Case "flash_list"
		      // Flash storage flight list response
		      Var theFlightCount As Integer = theJson.Lookup("count", 0)
		      Var theFlights() As Dictionary

		      If theJson.HasKey("flights") Then
		        Var theFlightsArray As JSONItem = theJson.Value("flights")
		        For i As Integer = 0 To theFlightsArray.Count - 1
		          Var theFlightJson As JSONItem = theFlightsArray.ValueAt(i)
		          Var theFlight As New Dictionary
		          theFlight.Value("slot") = theFlightJson.Lookup("slot", 0)
		          theFlight.Value("id") = theFlightJson.Lookup("id", 0)
		          theFlight.Value("altitude") = theFlightJson.Lookup("alt", 0.0)
		          theFlight.Value("time_ms") = theFlightJson.Lookup("time", 0)
		          theFlight.Value("samples") = theFlightJson.Lookup("samples", 0)
		          theFlights.Add(theFlight)
		        Next
		      End If

		      RaiseEvent FlashListReceived(theFlightCount, theFlights)

		    Case "flash_data"
		      // Flash data chunk response (samples)
		      Var theSlot As Integer = theJson.Lookup("slot", 0)
		      Var theStart As Integer = theJson.Lookup("start", 0)
		      Var theTotal As Integer = theJson.Lookup("total", 0)
		      Var theCount As Integer = theJson.Lookup("count", 0)
		      Var theHexData As String = theJson.Lookup("data", "")

		      RaiseEvent FlashDataReceived(theSlot, theStart, theTotal, theCount, theHexData)

		    Case "flash_header"
		      // Flash header response
		      Var theSlot As Integer = theJson.Lookup("slot", 0)
		      Var theHexData As String = theJson.Lookup("data", "")

		      RaiseEvent FlashHeaderReceived(theSlot, theHexData)

		    Case "fc_info"
		      // Flight computer device info
		      Var theInfo As New Dictionary
		      theInfo.Value("version") = theJson.Lookup("version", "")
		      theInfo.Value("build") = theJson.Lookup("build", "")
		      theInfo.Value("bmp390") = theJson.Lookup("bmp390", False)
		      theInfo.Value("lora") = theJson.Lookup("lora", False)
		      theInfo.Value("imu") = theJson.Lookup("imu", False)
		      theInfo.Value("oled") = theJson.Lookup("oled", False)
		      theInfo.Value("gps") = theJson.Lookup("gps", False)
		      theInfo.Value("state") = theJson.Lookup("state", "")
		      theInfo.Value("samples") = theJson.Lookup("samples", 0)
		      theInfo.Value("sd_free_kb") = theJson.Lookup("sd_free_kb", 0)
		      theInfo.Value("flight_count") = theJson.Lookup("flight_count", 0)
		      RaiseEvent DeviceInfoReceived(False, theInfo)

		    Case "gw_info"
		      // Gateway device info
		      LogMessage("Parsing gw_info message", "DEBUG")
		      Var theInfo As New Dictionary
		      theInfo.Value("version") = theJson.Lookup("version", "")
		      theInfo.Value("build") = theJson.Lookup("build", "")
		      theInfo.Value("lora") = theJson.Lookup("lora", False)
		      theInfo.Value("bmp390") = theJson.Lookup("bmp390", False)
		      theInfo.Value("gps") = theJson.Lookup("gps", False)
		      theInfo.Value("display") = theJson.Lookup("display", False)
		      theInfo.Value("connected") = theJson.Lookup("connected", False)
		      theInfo.Value("rx") = theJson.Lookup("rx", 0)
		      theInfo.Value("tx") = theJson.Lookup("tx", 0)
		      theInfo.Value("rssi") = theJson.Lookup("rssi", 0)
		      theInfo.Value("snr") = theJson.Lookup("snr", 0)
		      theInfo.Value("ground_pres") = theJson.Lookup("ground_pres", 0.0)
		      theInfo.Value("ground_temp") = theJson.Lookup("ground_temp", 0.0)
		      theInfo.Value("gps_fix") = theJson.Lookup("gps_fix", False)
		      theInfo.Value("gps_lat") = theJson.Lookup("gps_lat", 0.0)
		      theInfo.Value("gps_lon") = theJson.Lookup("gps_lon", 0.0)
		      theInfo.Value("gps_sats") = theJson.Lookup("gps_sats", 0)
		      LogMessage("Raising DeviceInfoReceived for gateway", "DEBUG")
		      RaiseEvent DeviceInfoReceived(True, theInfo)

		    Case "wifi_status"
		      // WiFi status response - forward to WiFi config window
		      If Window_WifiConfig.Visible Then
		        Window_WifiConfig.HandleWifiStatus(theJson)
		      End If

		    Case "wifi_list"
		      // WiFi network list response - forward to WiFi config window
		      If Window_WifiConfig.Visible Then
		        Window_WifiConfig.HandleWifiList(theJson)
		      End If

		    Case "gw_settings"
		      // Gateway settings response
		      Var theBacklight As Integer = theJson.Lookup("backlight", 0)
		      Var theBacklightMax As Integer = theJson.Lookup("backlight_max", 255)
		      Var theLoraTxPower As Integer = theJson.Lookup("lora_tx_power", 0)
		      Var theLoraTxMin As Integer = theJson.Lookup("lora_tx_power_min", 2)
		      Var theLoraTxMax As Integer = theJson.Lookup("lora_tx_power_max", 20)
		      Var theWifiTxPower As Integer = theJson.Lookup("wifi_tx_power", 0)
		      Var theWifiTxMin As Integer = theJson.Lookup("wifi_tx_power_min", 0)
		      Var theWifiTxMax As Integer = theJson.Lookup("wifi_tx_power_max", 20)
		      RaiseEvent GatewaySettingsReceived(theBacklight, theBacklightMax, theLoraTxPower, theLoraTxMin, theLoraTxMax, theWifiTxPower, theWifiTxMin, theWifiTxMax)

		    End Select

		  Catch theError As JSONException
		    LogMessage("JSON parse error: " + theError.Message, "ERROR")
		  Catch theError As RuntimeException
		    LogMessage("Parse error: " + theError.Message, "ERROR")
		  End Try
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ProcessReceivedData()
		  // Parse received JSON messages
		  Var theLineEnd As Integer = -1
		  Var theLfPos As Integer = pReceiveBuffer.IndexOf(Chr(10))
		  Var theCrPos As Integer = pReceiveBuffer.IndexOf(Chr(13))

		  If theLfPos >= 0 And theCrPos >= 0 Then
		    theLineEnd = Min(theLfPos, theCrPos)
		  ElseIf theLfPos >= 0 Then
		    theLineEnd = theLfPos
		  ElseIf theCrPos >= 0 Then
		    theLineEnd = theCrPos
		  End If

		  While theLineEnd >= 0
		    Var theLine As String = pReceiveBuffer.Left(theLineEnd)

		    Var theSkip As Integer = 1
		    If theLineEnd + 1 < pReceiveBuffer.Length Then
		      Var theNextChar As String = pReceiveBuffer.Middle(theLineEnd + 1, 1)
		      If theNextChar = Chr(10) Or theNextChar = Chr(13) Then
		        theSkip = 2
		      End If
		    End If

		    Var theRemainingLength As Integer = pReceiveBuffer.Length - theLineEnd - theSkip
		    If theRemainingLength > 0 Then
		      pReceiveBuffer = pReceiveBuffer.Right(theRemainingLength)
		    Else
		      pReceiveBuffer = ""
		    End If

		    If theLine.Trim <> "" Then
		      AccumulateJsonLine(theLine.Trim)
		    End If

		    theLfPos = pReceiveBuffer.IndexOf(Chr(10))
		    theCrPos = pReceiveBuffer.IndexOf(Chr(13))
		    If theLfPos >= 0 And theCrPos >= 0 Then
		      theLineEnd = Min(theLfPos, theCrPos)
		    ElseIf theLfPos >= 0 Then
		      theLineEnd = theLfPos
		    ElseIf theCrPos >= 0 Then
		      theLineEnd = theCrPos
		    Else
		      theLineEnd = -1
		    End If
		  Wend
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub AccumulateJsonLine(inLine As String)
		  // Accumulate JSON across multiple lines until complete
		  If inLine.Length >= 8 And inLine.Left(8) = "{""type"":" And pJsonAccumulator.Length > 0 Then
		    pJsonAccumulator = ""
		    pJsonBraceDepth = 0
		  End If

		  For i As Integer = 0 To inLine.Length - 1
		    Var theChar As String = inLine.Middle(i, 1)

		    If pJsonAccumulator = "" Then
		      If theChar = "{" Then
		        pJsonBraceDepth = 0
		      Else
		        Continue
		      End If
		    End If

		    pJsonAccumulator = pJsonAccumulator + theChar

		    Select Case theChar
		    Case "{"
		      pJsonBraceDepth = pJsonBraceDepth + 1
		    Case "}"
		      pJsonBraceDepth = pJsonBraceDepth - 1
		      If pJsonBraceDepth = 0 Then
		        Var theCompleteJson As String = pJsonAccumulator
		        pJsonAccumulator = ""
		        ParseMessage(theCompleteJson)
		        If i < inLine.Length - 1 Then
		          Var theRemainder As String = inLine.Middle(i + 1)
		          AccumulateJsonLine(theRemainder)
		          Return
		        End If
		      End If
		    End Select

		    If pJsonBraceDepth < 0 Then
		      pJsonAccumulator = ""
		      pJsonBraceDepth = 0
		    End If

		    If pJsonAccumulator.Length > 10000 Then
		      pJsonAccumulator = ""
		      pJsonBraceDepth = 0
		    End If
		  Next
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub SendData(inData As String)
		  // Send data via active connection (serial or TCP)
		  LogData("TX", inData)

		  If pSerialIsOpen And pSerialConnection <> Nil Then
		    pSerialConnection.Write(inData)
		    pSerialConnection.Flush
		  ElseIf pTcpIsConnected And pTcpSocket <> Nil Then
		    pTcpSocket.Write(inData)
		    pTcpSocket.Flush
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendArm()
		  // Send arm command to flight computer
		  SendCommand("arm")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDisarm()
		  // Send disarm command to flight computer
		  SendCommand("disarm")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendStatus()
		  // Request gateway status
		  SendCommand("status")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendPing()
		  // Ping the gateway
		  SendCommand("ping")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDownload()
		  // Request flight data download
		  SendCommand("download")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendReset()
		  // Reset the flight computer
		  SendCommand("reset")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendOrientationMode(inEnabled As Boolean)
		  // Enable or disable orientation testing mode (high-rate telemetry)
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "orientation_mode"
		  theJson.Value("id") = theId
		  theJson.Value("enabled") = inEnabled

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendRaw(inData As String)
		  // Send raw data (for debug console)
		  If Not IsConnected Then
		    LogMessage("Cannot send - not connected", "WARN")
		    Return
		  End If
		  SendData(inData + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendCommand(inCmd As String, inParam As Variant = Nil)
		  // Send JSON command with optional parameter
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = inCmd
		  theJson.Value("id") = theId

		  If inParam <> Nil Then
		    theJson.Value("param") = inParam
		  End If

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendStorageRead(inIsSd As Boolean, inFilename As String, inOffset As Integer)
		  // Send storage read command with filename and offset
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = If(inIsSd, "sd_read", "flash_read")
		  theJson.Value("id") = theId
		  theJson.Value("file") = inFilename
		  theJson.Value("offset") = inOffset

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendStorageDelete(inIsSd As Boolean, inFilename As String)
		  // Send storage delete command with filename
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = If(inIsSd, "sd_delete", "flash_delete")
		  theJson.Value("id") = theId
		  theJson.Value("file") = inFilename

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashList()
		  // Request list of stored flights from flash
		  SendCommand("flash_list")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashRead(inSlot As Integer, inSampleStart As Integer)
		  // Request flash data chunk (samples)
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "flash_read"
		  theJson.Value("id") = theId
		  theJson.Value("slot") = inSlot
		  theJson.Value("sample") = inSampleStart

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashReadHeader(inSlot As Integer)
		  // Request flash flight header
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "flash_read"
		  theJson.Value("id") = theId
		  theJson.Value("slot") = inSlot
		  theJson.Value("header") = True

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashDelete(inSlot As Integer)
		  // Delete a flight from flash (use slot=255 to delete all)
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "flash_delete"
		  theJson.Value("id") = theId
		  theJson.Value("slot") = inSlot

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendGetGatewaySettings()
		  // Request gateway settings (backlight, TX power, etc.)
		  SendCommand("gw_get_settings")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendGatewaySettings(inBacklight As Integer = -1, inLoraTxPower As Integer = -1, inWifiTxPower As Integer = -1)
		  // Set gateway settings (pass -1 to leave unchanged)
		  If Not IsConnected Then
		    LogMessage("Cannot send command - not connected", "WARN")
		    Return
		  End If

		  Var theId As Integer = pNextCommandId
		  pNextCommandId = pNextCommandId + 1

		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "gw_set"
		  theJson.Value("id") = theId

		  If inBacklight >= 0 Then
		    theJson.Value("backlight") = inBacklight
		  End If
		  If inLoraTxPower >= 0 Then
		    theJson.Value("lora_tx_power") = inLoraTxPower
		  End If
		  If inWifiTxPower >= 0 Then
		    theJson.Value("wifi_tx_power") = inWifiTxPower
		  End If

		  SendData(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod


	#tag Hook, Flags = &h0
		Event ConnectionChanged(inConnected As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event TelemetryReceived(inSample As TelemetrySample)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event LinkStatusChanged(inConnected As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event GatewayStatusReceived(inConnected As Boolean, inRxCount As Integer, inTxCount As Integer, inRssi As Integer, inSnr As Integer)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event AckReceived(inCommandId As Integer, inSuccess As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event ErrorReceived(inCode As String, inMessage As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event DebugLog(inMessage As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event RawMessageReceived(inMessage As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event StorageListReceived(inIsSd As Boolean, inFiles() As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event StorageDataReceived(inIsSd As Boolean, inOffset As Integer, inTotal As Integer, inData As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event StorageDeleteComplete(inIsSd As Boolean, inSuccess As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event DeviceInfoReceived(inIsGateway As Boolean, inInfo As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashListReceived(inCount As Integer, inFlights() As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashDataReceived(inSlot As Integer, inStart As Integer, inTotal As Integer, inCount As Integer, inData As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashHeaderReceived(inSlot As Integer, inData As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event GatewaySettingsReceived(inBacklight As Integer, inBacklightMax As Integer, inLoraTxPower As Integer, inLoraTxMin As Integer, inLoraTxMax As Integer, inWifiTxPower As Integer, inWifiTxMin As Integer, inWifiTxMax As Integer)
	#tag EndHook


	#tag Property, Flags = &h21
		Private pSerialConnection As SerialConnection
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pSerialIsOpen As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pReceiveBuffer As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pJsonAccumulator As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pJsonBraceDepth As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pNextCommandId As Integer = 1
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTcpSocket As TCPSocket
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTcpIsConnected As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pConnectionMode As ConnectionMode = ConnectionMode.Serial
	#tag EndProperty


End Class
#tag EndClass
