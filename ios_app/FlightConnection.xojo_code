#tag Class
Protected Class FlightConnection
	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize connection handler
		  pSocket = New TCPSocket

		  AddHandler pSocket.Connected, AddressOf HandleConnected
		  AddHandler pSocket.DataAvailable, AddressOf HandleDataAvailable
		  AddHandler pSocket.Error, AddressOf HandleError

		  pReceiveBuffer = ""
		  pCommandId = 0
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Destructor()
		  // Clean up resources
		  If pSocket <> Nil Then
		    RemoveHandler pSocket.Connected, AddressOf HandleConnected
		    RemoveHandler pSocket.DataAvailable, AddressOf HandleDataAvailable
		    RemoveHandler pSocket.Error, AddressOf HandleError
		    pSocket.Close
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Connect(inHost As String, inPort As Integer)
		  // Create a fresh socket (required in Xojo after close)
		  If pSocket <> Nil Then
		    RemoveHandler pSocket.Connected, AddressOf HandleConnected
		    RemoveHandler pSocket.DataAvailable, AddressOf HandleDataAvailable
		    RemoveHandler pSocket.Error, AddressOf HandleError
		    pSocket.Close
		  End If

		  pSocket = New TCPSocket
		  AddHandler pSocket.Connected, AddressOf HandleConnected
		  AddHandler pSocket.DataAvailable, AddressOf HandleDataAvailable
		  AddHandler pSocket.Error, AddressOf HandleError

		  pReceiveBuffer = ""
		  pSocket.Address = inHost
		  pSocket.Port = inPort
		  pSocket.Connect
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Disconnect()
		  // Disconnect from server
		  If pSocket <> Nil Then
		    pSocket.Close
		  End If
		  pReceiveBuffer = ""
		  RaiseEvent ConnectionChanged(False)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function IsConnected() As Boolean
		  If pSocket = Nil Then
		    Return False
		  End If
		  Return pSocket.IsConnected
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendArm(inRocketId As Integer)
		  // Send arm command to flight computer (requires rocket ID for safety)
		  SendCommandToRocket("arm", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDisarm(inRocketId As Integer)
		  // Send disarm command to flight computer (requires rocket ID for safety)
		  SendCommandToRocket("disarm", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendStatus(inRocketId As Integer)
		  // Request status from flight computer
		  SendCommandToRocket("status", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendReset(inRocketId As Integer)
		  // Reset the flight computer (requires rocket ID for safety)
		  SendCommandToRocket("reset", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDownload(inRocketId As Integer)
		  // Request flight data download
		  SendCommandToRocket("download", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashList(inRocketId As Integer)
		  // Request list of stored flights from flash
		  SendCommandToRocket("flash_list", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashRead(inRocketId As Integer, inSlot As Integer, inSample As Integer)
		  // Request flash data chunk (samples)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "flash_read"
		  theJson.Value("id") = pCommandId
		  theJson.Value("rocket") = inRocketId
		  theJson.Value("slot") = inSlot
		  theJson.Value("sample") = inSample

		  pSocket.Write(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashReadHeader(inRocketId As Integer, inSlot As Integer)
		  // Request flash flight header
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "flash_read"
		  theJson.Value("id") = pCommandId
		  theJson.Value("rocket") = inRocketId
		  theJson.Value("slot") = inSlot
		  theJson.Value("header") = True

		  pSocket.Write(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashDelete(inRocketId As Integer, inSlot As Integer)
		  // Delete a flight from flash (use slot=255 to delete all)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "flash_delete"
		  theJson.Value("id") = pCommandId
		  theJson.Value("rocket") = inRocketId
		  theJson.Value("slot") = inSlot

		  pSocket.Write(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendOrientationMode(inRocketId As Integer, inEnabled As Boolean)
		  // Enable or disable orientation testing mode (high-rate telemetry)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "orientation_mode"
		  theJson.Value("id") = pCommandId
		  theJson.Value("rocket") = inRocketId
		  theJson.Value("enabled") = inEnabled

		  pSocket.Write(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendSetRocketName(inRocketId As Integer, inName As String)
		  // Set the rocket name on the flight computer
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As New JSONItem
		  theJson.Value("cmd") = "set_rocket_name"
		  theJson.Value("id") = pCommandId
		  theJson.Value("rocket") = inRocketId
		  theJson.Value("name") = inName

		  pSocket.Write(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDeviceInfo(inRocketId As Integer)
		  // Request flight computer device info
		  SendCommandToRocket("fc_info", inRocketId)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendGatewayInfo()
		  // Request gateway device info (gateway command, no rocket ID needed)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As String = "{""cmd"":""gw_info"",""id"":" + Str(pCommandId) + "}" + EndOfLine
		  pSocket.Write(theJson)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendCommandToRocket(inCommand As String, inRocketId As Integer)
		  // Send a command with rocket ID to the gateway
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As String = "{""cmd"":""" + inCommand + """,""id"":" + Str(pCommandId) + ",""rocket"":" + Str(inRocketId) + "}" + EndOfLine
		  pSocket.Write(theJson)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendRequestRockets()
		  // Request list of active rockets from gateway (no rocket ID needed)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As String = "{""cmd"":""rockets"",""id"":" + Str(pCommandId) + "}" + EndOfLine
		  pSocket.Write(theJson)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleConnected(inSender As TCPSocket)
		  #Pragma Unused inSender
		  pReceiveBuffer = ""
		  RaiseEvent ConnectionChanged(True)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleDataAvailable(inSender As TCPSocket)
		  Var theData As String = inSender.ReadAll
		  pReceiveBuffer = pReceiveBuffer + theData
		  ProcessReceivedData()
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleError(inSender As TCPSocket, inError As RuntimeException)
		  #Pragma Unused inSender
		  #Pragma Unused inError
		  RaiseEvent ConnectionChanged(False)
		  RaiseEvent ErrorReceived("SOCKET_ERROR", "Connection error")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ProcessReceivedData()
		  Var theEolStr As String = EndOfLine
		  Var theLineEnd As Integer = pReceiveBuffer.IndexOf(theEolStr)

		  While theLineEnd >= 0
		    Var theLine As String = pReceiveBuffer.Left(theLineEnd)
		    Var theRemainingLength As Integer = pReceiveBuffer.Length - theLineEnd - theEolStr.Length
		    If theRemainingLength > 0 Then
		      pReceiveBuffer = pReceiveBuffer.Right(theRemainingLength)
		    Else
		      pReceiveBuffer = ""
		    End If

		    If theLine.Trim <> "" Then
		      ParseMessage(theLine.Trim)
		    End If

		    theLineEnd = pReceiveBuffer.IndexOf(theEolStr)
		  Wend
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ParseMessage(inJson As String)
		  If Not inJson.BeginsWith("{") Then
		    Return
		  End If

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
		      theSample.pRocketId = theJson.Lookup("id", 0)
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
		      theSample.pMaxAltitudeM = theJson.Lookup("max_alt", 0.0)
		      theSample.pMaxVelocityMps = theJson.Lookup("max_vel", 0.0)
		      theSample.pAccelMagnitude = theJson.Lookup("acc", 0.0)
		      theSample.pPyro1 = theJson.Lookup("pyro1", False)
		      theSample.pPyro2 = theJson.Lookup("pyro2", False)

		      // GPS data
		      theSample.pGpsLatitude = theJson.Lookup("lat", 0.0)
		      theSample.pGpsLongitude = theJson.Lookup("lon", 0.0)
		      theSample.pGpsSpeedMps = theJson.Lookup("gspd", 0.0)
		      theSample.pGpsHeadingDeg = theJson.Lookup("hdg", 0.0)
		      theSample.pGpsSatellites = theJson.Lookup("sats", 0)
		      theSample.pGpsFix = (theSample.pGpsSatellites >= 1)

		      // Accelerometer data — gateway sends in g's, convert to milli-g for storage
		      Var theAccelXg As Double = theJson.Lookup("ax", 0.0)
		      Var theAccelYg As Double = theJson.Lookup("ay", 0.0)
		      Var theAccelZg As Double = theJson.Lookup("az", 0.0)
		      theSample.pAccelX = Round(theAccelXg * 1000.0)
		      theSample.pAccelY = Round(theAccelYg * 1000.0)
		      theSample.pAccelZ = Round(theAccelZg * 1000.0)

		      // Gyroscope data — in dps
		      theSample.pGyroX = theJson.Lookup("gx", 0.0)
		      theSample.pGyroY = theJson.Lookup("gy", 0.0)
		      theSample.pGyroZ = theJson.Lookup("gz", 0.0)

		      // Magnetometer data — in milligauss
		      theSample.pMagX = theJson.Lookup("mx", 0.0)
		      theSample.pMagY = theJson.Lookup("my", 0.0)
		      theSample.pMagZ = theJson.Lookup("mz", 0.0)

		      // Calculate pitch and roll from accelerometer (degrees)
		      // IMU coordinate system: Y=up (vertical), X=right, Z=back
		      If theAccelYg <> 0.0 Or theAccelZg <> 0.0 Then
		        theSample.pPitch = ATan2(theAccelZg, theAccelYg) * 180.0 / 3.14159
		      End If
		      If theAccelYg <> 0.0 Or theAccelXg <> 0.0 Then
		        theSample.pRoll = ATan2(theAccelXg, theAccelYg) * 180.0 / 3.14159
		      End If

		      // Calculate heading from magnetometer
		      If theSample.pMagX <> 0.0 Or theSample.pMagY <> 0.0 Then
		        Var theHeading As Double = ATan2(theSample.pMagY, theSample.pMagX) * 180.0 / 3.14159
		        If theHeading < 0 Then theHeading = theHeading + 360.0
		        theSample.pHeading = theHeading
		      End If

		      RaiseEvent TelemetryReceived(theSample)

		    Case "link"
		      // Link status from gateway (LoRa connection to flight computer)
		      Var theStatus As String = theJson.Lookup("status", "")
		      If theStatus = "connected" Then
		        RaiseEvent LinkStatusChanged(True)
		      ElseIf theStatus = "lost" Then
		        RaiseEvent LinkStatusChanged(False)
		      End If

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
		      Var theCount As Integer = theJson.Lookup("count", 0)
		      Var theFlightsArray As JSONItem = theJson.Value("flights")
		      Var theFlights() As Dictionary
		      For i As Integer = 0 To theFlightsArray.Count - 1
		        Var theFlight As JSONItem = theFlightsArray.ChildAt(i)
		        Var theDict As New Dictionary
		        theDict.Value("slot") = theFlight.Lookup("slot", 0)
		        theDict.Value("altitude") = theFlight.Lookup("altitude", 0.0)
		        theDict.Value("time_ms") = theFlight.Lookup("time_ms", 0)
		        theDict.Value("samples") = theFlight.Lookup("samples", 0)
		        theFlights.Add(theDict)
		      Next
		      RaiseEvent FlashListReceived(theCount, theFlights)

		    Case "flash_data"
		      If theJson.Lookup("header", False) Then
		        // Flash header response
		        Var theSlot As Integer = theJson.Lookup("slot", 0)
		        Var theAltitude As Double = theJson.Lookup("altitude", 0.0)
		        Var theSamples As Integer = theJson.Lookup("samples", 0)
		        Var theTimeMs As Integer = theJson.Lookup("time_ms", 0)
		        RaiseEvent FlashHeaderReceived(theSlot, theAltitude, theSamples, theTimeMs)
		      Else
		        // Flash data chunk
		        Var theSlot As Integer = theJson.Lookup("slot", 0)
		        Var theStart As Integer = theJson.Lookup("start", 0)
		        Var theTotal As Integer = theJson.Lookup("total", 0)
		        Var theData As String = theJson.Lookup("data", "")
		        RaiseEvent FlashDataReceived(theSlot, theStart, theTotal, theData)
		      End If

		    Case "rockets"
		      Var theCount As Integer = theJson.Lookup("count", 0)
		      Var theRocketsArray As JSONItem = theJson.Value("rockets")
		      Var theRockets() As Dictionary
		      For i As Integer = 0 To theRocketsArray.Count - 1
		        Var theRocket As JSONItem = theRocketsArray.ChildAt(i)
		        Var theDict As New Dictionary
		        theDict.Value("id") = theRocket.Lookup("id", 0)
		        theDict.Value("lat") = theRocket.Lookup("lat", 0.0)
		        theDict.Value("lon") = theRocket.Lookup("lon", 0.0)
		        theDict.Value("alt") = theRocket.Lookup("alt", 0.0)
		        theDict.Value("state") = theRocket.Lookup("state", "")
		        theDict.Value("sats") = theRocket.Lookup("sats", 0)
		        theRockets.Add(theDict)
		      Next
		      RaiseEvent RocketsReceived(theCount, theRockets)

		    Case "fc_info"
		      // Flight computer device info
		      Var theInfo As New Dictionary
		      theInfo.Value("version") = theJson.Lookup("version", "")
		      theInfo.Value("build") = theJson.Lookup("build", "")
		      theInfo.Value("baro_type") = theJson.Lookup("baro_type", "")
		      theInfo.Value("imu_type") = theJson.Lookup("imu_type", "")
		      theInfo.Value("rocket_id") = theJson.Lookup("rocket_id", 0)
		      theInfo.Value("rocket_name") = theJson.Lookup("rocket_name", "")
		      theInfo.Value("state") = theJson.Lookup("state", "")
		      theInfo.Value("bmp390") = theJson.Lookup("bmp390", False)
		      theInfo.Value("lora") = theJson.Lookup("lora", False)
		      theInfo.Value("imu") = theJson.Lookup("imu", False)
		      theInfo.Value("gps") = theJson.Lookup("gps", False)
		      theInfo.Value("samples") = theJson.Lookup("samples", 0)
		      theInfo.Value("flight_count") = theJson.Lookup("flight_count", 0)
		      RaiseEvent DeviceInfoReceived(theInfo)

		    Case "gw_info"
		      // Gateway device info
		      Var theInfo As New Dictionary
		      theInfo.Value("version") = theJson.Lookup("version", "")
		      theInfo.Value("build") = theJson.Lookup("build", "")
		      theInfo.Value("gps_fix") = theJson.Lookup("gps_fix", False)
		      theInfo.Value("gps_sats") = theJson.Lookup("gps_sats", 0)
		      theInfo.Value("gps_lat") = theJson.Lookup("gps_lat", 0.0)
		      theInfo.Value("gps_lon") = theJson.Lookup("gps_lon", 0.0)
		      theInfo.Value("rx") = theJson.Lookup("rx", 0)
		      theInfo.Value("tx") = theJson.Lookup("tx", 0)
		      theInfo.Value("connected") = theJson.Lookup("connected", False)
		      theInfo.Value("ground_pres") = theJson.Lookup("ground_pres", 0.0)
		      theInfo.Value("ground_temp") = theJson.Lookup("ground_temp", 0.0)
		      RaiseEvent GatewayInfoReceived(theInfo)

		    End Select

		  Catch theError As JSONException
		    System.DebugLog("JSON parse error: " + theError.Message)
		  Catch theError As RuntimeException
		    System.DebugLog("Parse error: " + theError.Message)
		  End Try
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
		Event AckReceived(inCommandId As Integer, inSuccess As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event ErrorReceived(inCode As String, inMessage As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashListReceived(inCount As Integer, inFlights() As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashHeaderReceived(inSlot As Integer, inAltitude As Double, inSamples As Integer, inTimeMs As Integer)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashDataReceived(inSlot As Integer, inStart As Integer, inTotal As Integer, inData As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event RocketsReceived(inCount As Integer, inRockets() As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event DeviceInfoReceived(inInfo As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event GatewayInfoReceived(inInfo As Dictionary)
	#tag EndHook


	#tag Property, Flags = &h21
		Private pSocket As TCPSocket
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pReceiveBuffer As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pCommandId As Integer = 0
	#tag EndProperty


End Class
#tag EndClass
