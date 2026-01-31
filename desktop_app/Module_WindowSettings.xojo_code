#tag Module
Protected Module Module_WindowSettings
	#tag Method, Flags = &h21
		Private Function GetSettingsFile() As FolderItem
		  // Get the settings file location
		  Var theAppSupport As FolderItem = SpecialFolder.ApplicationData
		  If theAppSupport = Nil Then Return Nil
		  
		  Var theAppFolder As FolderItem = theAppSupport.Child("RocketAvionics")
		  If theAppFolder = Nil Then Return Nil
		  
		  Return theAppFolder.Child("settings.json")
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function LoadSettings() As Dictionary
		  // Load settings from JSON file
		  Var theFile As FolderItem = GetSettingsFile
		  If theFile = Nil Or Not theFile.Exists Then Return Nil
		  
		  Try
		    Var theInput As TextInputStream = TextInputStream.Open(theFile)
		    Var theContent As String = theInput.ReadAll
		    theInput.Close
		    
		    Return ParseJSON(theContent)
		  Catch e As IOException
		    Return Nil
		  Catch e As JSONException
		    Return Nil
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub LoadWindowPosition(theWindow As Window, theKey As String)
		  // Load saved window position from preferences file
		  Var theSettings As Dictionary = LoadSettings
		  If theSettings = Nil Then Return
		  
		  If Not theSettings.HasKey(theKey) Then Return
		  
		  Var theWindowSettings As Dictionary = theSettings.Value(theKey)
		  If theWindowSettings = Nil Then Return
		  
		  // Get saved values
		  Var theLeft As Integer = theWindowSettings.Lookup("Left", theWindow.Left)
		  Var theTop As Integer = theWindowSettings.Lookup("Top", theWindow.Top)
		  Var theWidth As Integer = theWindowSettings.Lookup("Width", theWindow.Width)
		  Var theHeight As Integer = theWindowSettings.Lookup("Height", theWindow.Height)
		  
		  // Validate that window is on screen
		  Var theScreen As Screen = Screen.ScreenAt(0)
		  If theScreen <> Nil Then
		    // Make sure window is at least partially visible
		    If theLeft < -theWidth + 50 Then theLeft = 50
		    If theTop < 0 Then theTop = 50
		    If theLeft > theScreen.AvailableWidth - 50 Then theLeft = theScreen.AvailableWidth - theWidth
		    If theTop > theScreen.AvailableHeight - 50 Then theTop = theScreen.AvailableHeight - theHeight
		  End If
		  
		  // Apply position
		  theWindow.Left = theLeft
		  theWindow.Top = theTop
		  theWindow.Width = theWidth
		  theWindow.Height = theHeight
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub SaveSettings(theSettings As Dictionary)
		  // Save settings to JSON file
		  Var theFile As FolderItem = GetSettingsFile
		  If theFile = Nil Then Return
		  
		  Try
		    // Ensure parent folder exists
		    Var theParent As FolderItem = theFile.Parent
		    If theParent <> Nil And Not theParent.Exists Then
		      theParent.CreateFolder
		    End If
		    
		    Var theOutput As TextOutputStream = TextOutputStream.Create(theFile)
		    theOutput.Write(GenerateJSON(theSettings))
		    theOutput.Close
		  Catch e As IOException
		    // Silently fail
		  End Try
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub SaveWindowPosition(theWindow As Window, theKey As String)
		  // Save window position to preferences file
		  Var theSettings As Dictionary = LoadSettings
		  If theSettings = Nil Then
		    theSettings = New Dictionary
		  End If
		  
		  // Create window settings dictionary
		  Var theWindowSettings As New Dictionary
		  theWindowSettings.Value("Left") = theWindow.Left
		  theWindowSettings.Value("Top") = theWindow.Top
		  theWindowSettings.Value("Width") = theWindow.Width
		  theWindowSettings.Value("Height") = theWindow.Height
		  
		  theSettings.Value(theKey) = theWindowSettings
		  
		  // Save to file
		  SaveSettings(theSettings)
		End Sub
	#tag EndMethod


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
	#tag EndViewBehavior
End Module
#tag EndModule
