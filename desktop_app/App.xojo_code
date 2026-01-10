#tag Class
Protected Class App
Inherits Application
	#tag Event
		Sub Close()
		  // Close database
		  If pDatabase <> Nil Then
		    pDatabase.Close
		  End If
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Initialize application

		  // Open database
		  pDatabase = New FlightDatabase
		  If Not pDatabase.Open() Then
		    MsgBox("Failed to open database: " + pDatabase.GetLastError)
		  End If

		  // Show main window
		  Window_Main.Show
		End Sub
	#tag EndEvent


	#tag MenuHandler
		Function DebugConsole() As Boolean Handles DebugConsole.Action
		  // Show debug console window
		  Window_Console.Show
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function DebugPing() As Boolean Handles DebugPing.Action
		  // Send ping command to gateway
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendPing()
		    Window_Console.LogMessage(">> Sending PING command")
		    Window_Console.Show
		  Else
		    MsgBox("Not connected to gateway")
		  End If
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function DebugStatus() As Boolean Handles DebugStatus.Action
		  // Request gateway status
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendStatus()
		    Window_Console.LogMessage(">> Requesting gateway STATUS")
		    Window_Console.Show
		  Else
		    MsgBox("Not connected to gateway")
		  End If
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function DebugSimulateTelemetry() As Boolean Handles DebugSimulateTelemetry.Action
		  // Simulate telemetry packets for testing UI
		  Window_Console.LogMessage(">> Simulating 10 telemetry packets...")
		  Window_Console.Show

		  For i As Integer = 1 To 10
		    Var theSample As New TelemetrySample
		    theSample.pTimeMs = i * 100
		    theSample.pAltitudeM = i * 15.5  // Simulated climb
		    theSample.pVelocityMps = 30.0 - (i * 2.0)  // Decreasing velocity
		    theSample.pPressurePa = 101325.0 - (i * 50.0)
		    theSample.pTemperatureC = 22.5
		    theSample.pState = If(i <= 3, "boost", If(i <= 6, "coast", "descent"))
		    theSample.pRssi = -60 - (i Mod 3)
		    theSample.pSnr = 8 + (i Mod 3)

		    // Use public method to add sample and update display
		    Window_Main.SimulateTelemetrySample(theSample)

		    // Log the simulated packet
		    Window_Console.LogMessage("<< SIM: alt=" + Str(theSample.pAltitudeM) + "m vel=" + Str(theSample.pVelocityMps) + "m/s state=" + theSample.pState)
		  Next

		  Window_Console.LogMessage(">> Simulation complete - 10 packets added")
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FileClose() As Boolean Handles FileClose.Action
		  // Close the frontmost window
		  Var theWindow As Window = Window(0)
		  If theWindow <> Nil Then
		    theWindow.Close
		  End If
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function WindowBringAllToFront() As Boolean Handles WindowBringAllToFront.Action
		  // Bring all windows to front
		  For i As Integer = 0 To WindowCount - 1
		    Window(i).Show
		  Next
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FlightMainWindow() As Boolean Handles FlightMainWindow.Action
		  // Show main window
		  Window_Main.Show
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FlightHistory() As Boolean Handles FlightHistory.Action
		  // Show flight history window
		  Window_History.Show
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FlightCompare() As Boolean Handles FlightCompare.Action
		  // Show flight comparison window
		  Window_FlightCompare.Show
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FlightSaveCurrent() As Boolean Handles FlightSaveCurrent.Action
		  // Save current flight from main window
		  If Window_Main.pCurrentFlight <> Nil And Window_Main.pCurrentFlight.GetSampleCount > 0 Then
		    If pDatabase.SaveFlight(Window_Main.pCurrentFlight) Then
		      MsgBox("Flight saved successfully.")
		      Window_History.RefreshFlightList
		    Else
		      MsgBox("Failed to save flight: " + pDatabase.GetLastError)
		    End If
		  Else
		    MsgBox("No flight data to save.")
		  End If
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FilePageSetup() As Boolean Handles FilePageSetup.Action
		  // Show page setup dialog
		  Module_PrintReport.ShowPageSetup
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FilePrint() As Boolean Handles FilePrint.Action
		  // Print from active window
		  Var theWindow As Window = Window(0)
		  If theWindow IsA Window_Main Then
		    If Window_Main.pCurrentFlight <> Nil And Window_Main.pCurrentFlight.GetSampleCount > 0 Then
		      Module_PrintReport.PrintFlight(Window_Main.pCurrentFlight)
		    Else
		      MsgBox("No flight data to print.")
		    End If
		  ElseIf theWindow IsA Window_History Then
		    Var theFlights() As FlightData = Window_History.GetSelectedFlights
		    If theFlights.Count > 0 Then
		      If theFlights.Count = 1 Then
		        Module_PrintReport.PrintFlight(theFlights(0))
		      Else
		        Module_PrintReport.PrintMultiFlight(theFlights)
		      End If
		    Else
		      MsgBox("No flights selected to print.")
		    End If
		  Else
		    MsgBox("No printable content in active window.")
		  End If
		  Return True
		End Function
	#tag EndMenuHandler


	#tag Property, Flags = &h0
		pDatabase As FlightDatabase
	#tag EndProperty


	#tag Constant, Name = kEditClear, Type = String, Dynamic = False, Default = \"&Delete", Scope = Public
		#Tag Instance, Platform = Windows, Language = Default, Definition  = \"&Delete"
		#Tag Instance, Platform = Linux, Language = Default, Definition  = \"&Delete"
	#tag EndConstant

	#tag Constant, Name = kFileQuit, Type = String, Dynamic = False, Default = \"&Quit", Scope = Public
		#Tag Instance, Platform = Windows, Language = Default, Definition  = \"E&xit"
	#tag EndConstant

	#tag Constant, Name = kFileQuitShortcut, Type = String, Dynamic = False, Default = \"", Scope = Public
		#Tag Instance, Platform = Mac OS, Language = Default, Definition  = \"Cmd+Q"
		#Tag Instance, Platform = Linux, Language = Default, Definition  = \"Ctrl+Q"
	#tag EndConstant


	#tag ViewBehavior
	#tag EndViewBehavior
End Class
#tag EndClass
