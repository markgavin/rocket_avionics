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

		  // Load preferences
		  Module_Preferences.LoadPreferences

		  // Open database
		  pDatabase = New FlightDatabase
		  If Not pDatabase.Open() Then
		    MsgBox("Failed to open database: " + pDatabase.GetLastError)
		  End If

		  // Splash screen is shown from Window_Main.Open() so it appears centered over the main window
		End Sub
	#tag EndEvent


	#tag MenuHandler
		Function AppPreferences() As Boolean Handles AppPreferences.Action
		  // Show Preferences dialog
		  Dialog_Preferences.ShowModal
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function AppAbout() As Boolean Handles AppAbout.Action
		  // Show About dialog
		  Dialog_About.ShowModal
		  Return True
		End Function
	#tag EndMenuHandler

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
		Function DebugShowBMP390Details() As Boolean Handles DebugShowBMP390Details.Action
		  // Toggle BMP390 detail display in console
		  Window_Console.pShowBMP390Details = Not Window_Console.pShowBMP390Details
		  DebugShowBMP390Details.Checked = Window_Console.pShowBMP390Details
		  Window_Console.LogMessage("** BMP390 details " + If(Window_Console.pShowBMP390Details, "enabled", "disabled"))
		  Window_Console.Show
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
		Function FileExportCSV() As Boolean Handles FileExportCSV.Action
		  // Export current flight as CSV
		  Var theFlight As FlightData = Nil

		  // Get flight from active window
		  Var theWindow As Window = Window(0)
		  If theWindow IsA Window_Main Then
		    theFlight = Window_Main.pCurrentFlight
		  ElseIf theWindow IsA Window_History Then
		    Var theFlights() As FlightData = Window_History.GetSelectedFlights
		    If theFlights.Count > 0 Then
		      theFlight = theFlights(0)
		    End If
		  End If

		  If theFlight = Nil Or theFlight.GetSampleCount = 0 Then
		    MsgBox("No flight data to export.")
		    Return True
		  End If

		  // Show save dialog
		  Var theDialog As New SaveFileDialog
		  theDialog.Title = "Export Flight as CSV"

		  Var theCsvType As New FileType
		  theCsvType.Name = "CSV Files"
		  theCsvType.Extensions = "csv"
		  theDialog.Filter = theCsvType

		  theDialog.SuggestedFileName = "flight_" + theFlight.pFlightId.Left(8) + ".csv"

		  Var theFile As FolderItem = theDialog.ShowModal

		  If theFile <> Nil Then
		    Try
		      Var theStream As TextOutputStream = TextOutputStream.Create(theFile)
		      theStream.Write(theFlight.ToCSV)
		      theStream.Close
		      MsgBox("Flight exported to CSV successfully.")
		    Catch theError As IOException
		      MsgBox("Failed to export: " + theError.Message)
		    End Try
		  End If

		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FileExportJSON() As Boolean Handles FileExportJSON.Action
		  // Export current flight as JSON
		  Var theFlight As FlightData = Nil

		  // Get flight from active window
		  Var theWindow As Window = Window(0)
		  If theWindow IsA Window_Main Then
		    theFlight = Window_Main.pCurrentFlight
		  ElseIf theWindow IsA Window_History Then
		    Var theFlights() As FlightData = Window_History.GetSelectedFlights
		    If theFlights.Count > 0 Then
		      theFlight = theFlights(0)
		    End If
		  End If

		  If theFlight = Nil Or theFlight.GetSampleCount = 0 Then
		    MsgBox("No flight data to export.")
		    Return True
		  End If

		  // Show save dialog
		  Var theDialog As New SaveFileDialog
		  theDialog.Title = "Export Flight as JSON"

		  Var theJsonType As New FileType
		  theJsonType.Name = "JSON Files"
		  theJsonType.Extensions = "json"
		  theDialog.Filter = theJsonType

		  theDialog.SuggestedFileName = "flight_" + theFlight.pFlightId.Left(8) + ".json"

		  Var theFile As FolderItem = theDialog.ShowModal

		  If theFile <> Nil Then
		    Try
		      Var theStream As TextOutputStream = TextOutputStream.Create(theFile)
		      theStream.Write(theFlight.ToJSON)
		      theStream.Close
		      MsgBox("Flight exported to JSON successfully.")
		    Catch theError As IOException
		      MsgBox("Failed to export: " + theError.Message)
		    End Try
		  End If

		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FileImportJSON() As Boolean Handles FileImportJSON.Action
		  // Import flight from JSON file
		  Var theDialog As New OpenFileDialog
		  theDialog.Title = "Import Flight from JSON"

		  Var theJsonType As New FileType
		  theJsonType.Name = "JSON Files"
		  theJsonType.Extensions = "json"
		  theDialog.Filter = theJsonType

		  Var theFile As FolderItem = theDialog.ShowModal

		  If theFile <> Nil And theFile.Exists Then
		    Try
		      Var theStream As TextInputStream = TextInputStream.Open(theFile)
		      Var theContent As String = theStream.ReadAll
		      theStream.Close

		      Var theFlight As FlightData = FlightData.FromJSON(theContent)

		      If theFlight <> Nil Then
		        // Save to database
		        If pDatabase.SaveFlight(theFlight) Then
		          MsgBox("Flight imported successfully." + EndOfLine + _
		            "Max Altitude: " + Format(theFlight.pMaxAltitudeM, "0.0") + " m" + EndOfLine + _
		            "Samples: " + Str(theFlight.GetSampleCount))
		          Window_History.RefreshFlightList
		        Else
		          MsgBox("Failed to save imported flight: " + pDatabase.GetLastError)
		        End If
		      Else
		        MsgBox("Failed to parse JSON file.")
		      End If

		    Catch theError As IOException
		      MsgBox("Failed to read file: " + theError.Message)
		    End Try
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

	#tag MenuHandler
		Function FlightCompare() As Boolean Handles FlightCompare.Action
		  // Show flight comparison window
		  Window_FlightCompare.Show
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function FlightDeviceStorage() As Boolean Handles FlightDeviceStorage.Action
		  // Show device storage browser
		  Window_DeviceStorage.Show
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
		Function FlightMainWindow() As Boolean Handles FlightMainWindow.Action
		  // Show main window
		  Window_Main.Show
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
		Function ViewFlightComputerInfo() As Boolean Handles ViewFlightComputerInfo.Action
		  // Request and display flight computer device information
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Console.LogMessage(">> Requesting flight computer info...")
		    Window_Main.pConnection.SendCommand("info")
		  Else
		    MsgBox("Not connected to gateway")
		  End If
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function ViewGatewayInfo() As Boolean Handles ViewGatewayInfo.Action
		  // Request and display gateway device information
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Console.LogMessage(">> Requesting gateway device info...")
		    Window_Main.pConnection.SendCommand("gw_info")
		  Else
		    MsgBox("Not connected to gateway")
		  End If
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function ViewOrientation() As Boolean Handles ViewOrientation.Action
		  // Show orientation visualization window
		  Window_Orientation.Show
		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function ViewWifiConfig() As Boolean Handles ViewWifiConfig.Action
		  // Show WiFi configuration window
		  Window_WifiConfig.Show
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
		Function HelpUserGuide() As Boolean Handles HelpUserGuide.Action
		  // Open the User Guide HTML file in the default browser
		  Var theHelpFile As FolderItem = Nil

		  // Try macOS app bundle Resources folder
		  #If TargetMacOS Then
		    theHelpFile = App.ExecutableFile.Parent.Parent.Child("Resources").Child("Help.html")
		  #EndIf

		  // Fallback: look in app bundle for macOS (alternate location)
		  If theHelpFile = Nil Or Not theHelpFile.Exists Then
		    theHelpFile = App.ExecutableFile.Parent.Parent.Child("Help.html")
		  End If

		  // Fallback: look next to executable (Windows/Linux)
		  If theHelpFile = Nil Or Not theHelpFile.Exists Then
		    theHelpFile = App.ExecutableFile.Parent.Child("Help.html")
		  End If

		  // Fallback: look in desktop_app source folder (for IDE debugging)
		  If theHelpFile = Nil Or Not theHelpFile.Exists Then
		    theHelpFile = New FolderItem("/Users/markgavin/mg/rocket_avionics/desktop_app/Help.html", FolderItem.PathModes.Native)
		  End If

		  If theHelpFile <> Nil And theHelpFile.Exists Then
		    theHelpFile.Open
		  Else
		    Var theDialog As New MessageDialog
		    theDialog.Message = "Help file not found."
		    theDialog.IconType = MessageDialog.IconTypes.Caution
		    Call theDialog.ShowModal
		  End If

		  Return True
		End Function
	#tag EndMenuHandler

	#tag MenuHandler
		Function HelpAbout() As Boolean Handles HelpAbout.Action
		  // Show About dialog (from Help menu)
		  Dialog_About.ShowModal
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
