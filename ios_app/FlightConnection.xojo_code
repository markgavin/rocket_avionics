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
		  // Connect to gateway
		  If pSocket = Nil Then
		    Return
		  End If

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
		Sub SendArm()
		  SendCommand("arm")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDisarm()
		  SendCommand("disarm")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendStatus()
		  SendCommand("status")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDownload()
		  SendCommand("download")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashList()
		  SendCommand("flash_list")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashRead(inSlot As Integer, inSample As Integer)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As String = "{""cmd"":""flash_read"",""id"":" + Str(pCommandId) + ",""slot"":" + Str(inSlot) + ",""sample"":" + Str(inSample) + "}" + EndOfLine
		  pSocket.Write(theJson)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendFlashDelete(inSlot As Integer)
		  If pSocket = Nil Or Not pSocket.IsConnected Then
		    Return
		  End If

		  pCommandId = pCommandId + 1
		  Var theJson As String = "{""cmd"":""flash_delete"",""id"":" + Str(pCommandId) + ",""slot"":" + Str(inSlot) + "}" + EndOfLine
		  pSocket.Write(theJson)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub SendCommand(inCommand As String)
		  SendCommandToRocket(inCommand, 0)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendCommandToRocket(inCommand As String, inRocketId As Integer)
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
		  SendCommand("rockets")
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
		      // Telemetry sample
		      Var theSample As New TelemetrySample
		      theSample.pRocketId = theJson.Lookup("id", 0)
		      theSample.pAltitudeM = theJson.Lookup("alt", 0.0)
		      theSample.pVelocityMps = theJson.Lookup("vel", 0.0)
		      theSample.pState = theJson.Lookup("state", "")
		      theSample.pGpsLatitude = theJson.Lookup("lat", 0.0)
		      theSample.pGpsLongitude = theJson.Lookup("lon", 0.0)
		      theSample.pGpsSatellites = theJson.Lookup("sats", 0)
		      theSample.pGpsFix = (theSample.pGpsSatellites > 0)
		      theSample.pRssi = theJson.Lookup("rssi", 0)
		      theSample.pSnr = theJson.Lookup("snr", 0)
		      RaiseEvent TelemetryReceived(theSample)

		    Case "link"
		      Var theConnected As Boolean = theJson.Lookup("connected", False)
		      RaiseEvent LinkStatusChanged(theConnected)

		    Case "ack"
		      Var theCmd As String = theJson.Lookup("cmd", "")
		      Var theSuccess As Boolean = theJson.Lookup("success", False)
		      RaiseEvent CommandAcknowledged(theCmd, theSuccess)

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
		      Var theSlot As Integer = theJson.Lookup("slot", 0)
		      Var theStart As Integer = theJson.Lookup("start", 0)
		      Var theTotal As Integer = theJson.Lookup("total", 0)
		      Var theData As String = theJson.Lookup("data", "")
		      RaiseEvent FlashDataReceived(theSlot, theStart, theTotal, theData)

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
		Event CommandAcknowledged(inCommand As String, inSuccess As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event ErrorReceived(inCode As String, inMessage As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashListReceived(inCount As Integer, inFlights() As Dictionary)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event FlashDataReceived(inSlot As Integer, inStart As Integer, inTotal As Integer, inData As String)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event RocketsReceived(inCount As Integer, inRockets() As Dictionary)
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
