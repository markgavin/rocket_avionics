#tag Module
Protected Module Module_Preferences
	#tag Method, Flags = &h0
		Function GetAltitudeDisplay(inMeters As Double) As String
		  // Format altitude for display based on units preference
		  Select Case AltitudeUnits
		  Case "feet"
		    Return Format(inMeters * 3.28084, "0") + " ft"
		  Case "meters"
		    Return Format(inMeters, "0.0") + " m"
		  Else
		    Return Format(inMeters, "0.0") + " m"
		  End Select
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function GetPreferencesFile() As FolderItem
		  // Get the preferences file path
		  Var theAppSupport As FolderItem = SpecialFolder.ApplicationData.Child("RocketAvionics")
		  
		  If Not theAppSupport.Exists Then
		    theAppSupport.CreateFolder
		  End If
		  
		  Return theAppSupport.Child("preferences.json")
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetStringPref(inKey As String, inDefault As String = "") As String
		  // Get a string preference value
		  If pStringPrefs = Nil Then
		    pStringPrefs = New Dictionary
		  End If
		  
		  If pStringPrefs.HasKey(inKey) Then
		    Return pStringPrefs.Value(inKey)
		  Else
		    Return inDefault
		  End If
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetVelocityDisplay(inMps As Double) As String
		  // Format velocity for display based on units preference
		  Select Case VelocityUnits
		  Case "fps"
		    Return Format(inMps * 3.28084, "0") + " ft/s"
		  Case "mph"
		    Return Format(inMps * 2.23694, "0") + " mph"
		  Case "mps"
		    Return Format(inMps, "0.0") + " m/s"
		  Else
		    Return Format(inMps, "0.0") + " m/s"
		  End Select
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub LoadPreferences()
		  // Load preferences from JSON file
		  Var theFile As FolderItem = GetPreferencesFile()
		  
		  If theFile <> Nil And theFile.Exists Then
		    Try
		      Var theStream As TextInputStream = TextInputStream.Open(theFile)
		      Var theContent As String = theStream.ReadAll
		      theStream.Close
		      
		      Var theJson As New JSONItem(theContent)
		      
		      If theJson.HasKey("default_serial_port") Then
		        DefaultSerialPort = theJson.Value("default_serial_port")
		      End If
		      If theJson.HasKey("altitude_units") Then
		        AltitudeUnits = theJson.Value("altitude_units")
		      End If
		      If theJson.HasKey("velocity_units") Then
		        VelocityUnits = theJson.Value("velocity_units")
		      End If
		      If theJson.HasKey("auto_save_flights") Then
		        AutoSaveFlights = theJson.Value("auto_save_flights")
		      End If
		      If theJson.HasKey("show_grid") Then
		        ShowGrid = theJson.Value("show_grid")
		      End If
		      If theJson.HasKey("show_apogee_marker") Then
		        ShowApogeeMarker = theJson.Value("show_apogee_marker")
		      End If
		      If theJson.HasKey("pilot_name") Then
		        DefaultPilotName = theJson.Value("pilot_name")
		      End If
		      If theJson.HasKey("rocket_name") Then
		        DefaultRocketName = theJson.Value("rocket_name")
		      End If
		      If theJson.HasKey("location") Then
		        DefaultLocation = theJson.Value("location")
		      End If
		      
		      // WiFi settings
		      If theJson.HasKey("connection_type") Then
		        ConnectionType = theJson.Value("connection_type")
		      End If
		      If theJson.HasKey("wifi_host") Then
		        WifiHost = theJson.Value("wifi_host")
		      End If
		      If theJson.HasKey("wifi_port") Then
		        WifiPort = theJson.Value("wifi_port")
		      End If
		      
		      // String preferences (rocket names, etc.)
		      If theJson.HasKey("string_prefs") Then
		        pStringPrefs = New Dictionary
		        Var thePrefsJson As JSONItem = theJson.Value("string_prefs")
		        For Each theKey As Variant In thePrefsJson.Keys
		          pStringPrefs.Value(theKey.StringValue) = thePrefsJson.Value(theKey.StringValue)
		        Next
		      End If
		      
		    Catch theError As IOException
		      // Use defaults if file can't be read
		    Catch theError As JSONException
		      // Use defaults if JSON is invalid
		    End Try
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SavePreferences()
		  // Save preferences to JSON file
		  Var theFile As FolderItem = GetPreferencesFile()
		  
		  If theFile <> Nil Then
		    Try
		      Var theJson As New JSONItem
		      theJson.Value("default_serial_port") = DefaultSerialPort
		      theJson.Value("altitude_units") = AltitudeUnits
		      theJson.Value("velocity_units") = VelocityUnits
		      theJson.Value("auto_save_flights") = AutoSaveFlights
		      theJson.Value("show_grid") = ShowGrid
		      theJson.Value("show_apogee_marker") = ShowApogeeMarker
		      theJson.Value("pilot_name") = DefaultPilotName
		      theJson.Value("rocket_name") = DefaultRocketName
		      theJson.Value("location") = DefaultLocation
		      
		      // WiFi settings
		      theJson.Value("connection_type") = ConnectionType
		      theJson.Value("wifi_host") = WifiHost
		      theJson.Value("wifi_port") = WifiPort
		      
		      // String preferences (rocket names, etc.)
		      If pStringPrefs <> Nil And pStringPrefs.KeyCount > 0 Then
		        Var thePrefsJson As New JSONItem
		        For Each theKey As Variant In pStringPrefs.Keys
		          thePrefsJson.Value(theKey.StringValue) = pStringPrefs.Value(theKey)
		        Next
		        theJson.Value("string_prefs") = thePrefsJson
		      End If
		      
		      Var theStream As TextOutputStream = TextOutputStream.Create(theFile)
		      theStream.Write(theJson.ToString)
		      theStream.Close
		      
		    Catch theError As IOException
		      // Silently fail if preferences can't be saved
		    End Try
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SetStringPref(inKey As String, inValue As String)
		  // Set a string preference value
		  If pStringPrefs = Nil Then
		    pStringPrefs = New Dictionary
		  End If
		  
		  pStringPrefs.Value(inKey) = inValue
		  SavePreferences
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h0
		AltitudeUnits As String = "meters"
	#tag EndProperty

	#tag Property, Flags = &h0
		AutoSaveFlights As Boolean = True
	#tag EndProperty

	#tag Property, Flags = &h0
		ConnectionType As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		DefaultLocation As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		DefaultPilotName As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		DefaultRocketName As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		DefaultSerialPort As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pStringPrefs As Dictionary
	#tag EndProperty

	#tag Property, Flags = &h0
		ShowApogeeMarker As Boolean = True
	#tag EndProperty

	#tag Property, Flags = &h0
		ShowGrid As Boolean = True
	#tag EndProperty

	#tag Property, Flags = &h0
		VelocityUnits As String = "mps"
	#tag EndProperty

	#tag Property, Flags = &h0
		WifiHost As String = "192.168.4.1"
	#tag EndProperty

	#tag Property, Flags = &h0
		WifiPort As Integer = 8080
	#tag EndProperty


	#tag ViewBehavior
		#tag ViewProperty
			Name="Name"
			Visible=true
			Group="ID"
			InitialValue=""
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="Index"
			Visible=true
			Group="ID"
			InitialValue="-2147483648"
			Type="Integer"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="Super"
			Visible=true
			Group="ID"
			InitialValue=""
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="Left"
			Visible=true
			Group="Position"
			InitialValue="0"
			Type="Integer"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="Top"
			Visible=true
			Group="Position"
			InitialValue="0"
			Type="Integer"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="DefaultSerialPort"
			Visible=false
			Group="Behavior"
			InitialValue=""
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="AltitudeUnits"
			Visible=false
			Group="Behavior"
			InitialValue="meters"
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="VelocityUnits"
			Visible=false
			Group="Behavior"
			InitialValue="mps"
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="AutoSaveFlights"
			Visible=false
			Group="Behavior"
			InitialValue="True"
			Type="Boolean"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="ShowGrid"
			Visible=false
			Group="Behavior"
			InitialValue="True"
			Type="Boolean"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="ShowApogeeMarker"
			Visible=false
			Group="Behavior"
			InitialValue="True"
			Type="Boolean"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="DefaultPilotName"
			Visible=false
			Group="Behavior"
			InitialValue=""
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="DefaultRocketName"
			Visible=false
			Group="Behavior"
			InitialValue=""
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="DefaultLocation"
			Visible=false
			Group="Behavior"
			InitialValue=""
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="ConnectionType"
			Visible=false
			Group="Behavior"
			InitialValue="0"
			Type="Integer"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="WifiHost"
			Visible=false
			Group="Behavior"
			InitialValue="192.168.4.1"
			Type="String"
			EditorType=""
		#tag EndViewProperty
		#tag ViewProperty
			Name="WifiPort"
			Visible=false
			Group="Behavior"
			InitialValue="8080"
			Type="Integer"
			EditorType=""
		#tag EndViewProperty
	#tag EndViewBehavior
End Module
#tag EndModule
