#tag Class
Protected Class FlightConnection
	#tag Method, Flags = &h0
		Function Connect(inPortName As String) As Boolean
		  // Connect to gateway via USB Serial
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
		Sub Constructor()
		  // Initialize connection handler
		  pSerialConnection = New SerialConnection
		  pSerialConnection.Baud = SerialConnection.Baud115200
		  pSerialConnection.Bits = SerialConnection.Bits8
		  pSerialConnection.Parity = SerialConnection.Parities.None
		  pSerialConnection.StopBit = SerialConnection.StopBits.One
		  AddHandler pSerialConnection.DataReceived, AddressOf HandleSerialDataReceived
		  AddHandler pSerialConnection.Error, AddressOf HandleSerialConnectionError

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
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Disconnect()
		  // Close connection
		  LogMessage("Disconnecting", "INFO")

		  If pSerialIsOpen And pSerialConnection <> Nil Then
		    pSerialConnection.Close
		  End If
		  pSerialIsOpen = False

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

	#tag Method, Flags = &h0
		Function IsConnected() As Boolean
		  If pSerialConnection = Nil Then
		    Return False
		  End If
		  Return pSerialIsOpen
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
		      RaiseEvent TelemetryReceived(theSample)

		    Case "link"
		      // Link status from gateway
		      Var theStatus As String = theJson.Lookup("status", "")
		      RaiseEvent LinkStatusChanged(theStatus = "connected")

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

		    Case "sd_list", "flash_list"
		      // Storage file list response
		      Var theIsSd As Boolean = (theType = "sd_list")
		      Var theFiles() As Dictionary

		      If theJson.HasKey("files") Then
		        Var theFilesArray As JSONItem = theJson.Value("files")
		        For i As Integer = 0 To theFilesArray.Count - 1
		          Var theFileJson As JSONItem = theFilesArray.ValueAt(i)
		          Var theFile As New Dictionary
		          theFile.Value("name") = theFileJson.Lookup("name", "")
		          theFile.Value("size") = theFileJson.Lookup("size", 0)
		          theFile.Value("date") = theFileJson.Lookup("date", "")
		          theFiles.Add(theFile)
		        Next
		      End If

		      RaiseEvent StorageListReceived(theIsSd, theFiles)

		    Case "sd_data", "flash_data"
		      // Storage data chunk response
		      Var theIsSd As Boolean = (theType = "sd_data")
		      Var theOffset As Integer = theJson.Lookup("offset", 0)
		      Var theTotal As Integer = theJson.Lookup("total", 0)
		      Var theHexData As String = theJson.Lookup("data", "")

		      RaiseEvent StorageDataReceived(theIsSd, theOffset, theTotal, theHexData)

		    Case "fc_info"
		      // Flight computer device info
		      Var theInfo As New Dictionary
		      theInfo.Value("version") = theJson.Lookup("version", "")
		      theInfo.Value("build") = theJson.Lookup("build", "")
		      theInfo.Value("bmp390") = theJson.Lookup("bmp390", False)
		      theInfo.Value("lora") = theJson.Lookup("lora", False)
		      theInfo.Value("sd") = theJson.Lookup("sd", False)
		      theInfo.Value("rtc") = theJson.Lookup("rtc", False)
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
		      theInfo.Value("display") = theJson.Lookup("display", False)
		      theInfo.Value("connected") = theJson.Lookup("connected", False)
		      theInfo.Value("rx") = theJson.Lookup("rx", 0)
		      theInfo.Value("tx") = theJson.Lookup("tx", 0)
		      theInfo.Value("rssi") = theJson.Lookup("rssi", 0)
		      theInfo.Value("snr") = theJson.Lookup("snr", 0)
		      theInfo.Value("ground_pres") = theJson.Lookup("ground_pres", 0.0)
		      theInfo.Value("ground_temp") = theJson.Lookup("ground_temp", 0.0)
		      LogMessage("Raising DeviceInfoReceived for gateway", "DEBUG")
		      RaiseEvent DeviceInfoReceived(True, theInfo)

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
		  // Send data via serial connection
		  LogData("TX", inData)

		  If pSerialIsOpen And pSerialConnection <> Nil Then
		    pSerialConnection.Write(inData)
		    pSerialConnection.Flush
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


End Class
#tag EndClass
