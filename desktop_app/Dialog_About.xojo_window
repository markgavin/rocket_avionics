#tag Window
Begin Window Dialog_About
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   1
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   433
   ImplicitInstance=   True
   LiveResize      =   "False"
   MacProcID       =   0
   MaxHeight       =   433
   MaximizeButton  =   False
   MaxWidth        =   400
   MenuBar         =   0
   MenuBarVisible  =   False
   MinHeight       =   433
   MinimizeButton  =   False
   MinWidth        =   400
   Placement       =   1
   Resizeable      =   False
   Title           =   "About Rocket Avionics"
   Visible         =   True
   Width           =   400
   Begin Canvas CanvasIcon
      AllowAutoDeactivate=   True
      AllowFocus      =   False
      AllowFocusRing  =   False
      AllowTabs       =   False
      Backdrop        =   0
      DoubleBuffer    =   False
      Enabled         =   True
      Height          =   256
      Index           =   -2147483648
      InitialParent   =   ""
      Left            =   72
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   0
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   False
      Tooltip         =   ""
      Top             =   20
      Transparent     =   True
      Visible         =   True
      Width           =   256
   End
   Begin Label LabelAppName
      AutoDeactivate  =   True
      Bold            =   True
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      Height          =   25
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Rocket Avionics Ground Station"
      TextAlign       =   1
      TextColor       =   &c00000000
      TextFont        =   "System"
      TextSize        =   18.0
      TextUnit        =   0
      Top             =   290
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   360
   End
   Begin Label LabelVersion
      AutoDeactivate  =   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      Height          =   20
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Version 1.0"
      TextAlign       =   1
      TextColor       =   &c66666600
      TextFont        =   "System"
      TextSize        =   12.0
      TextUnit        =   0
      Top             =   318
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   360
   End
   Begin Label LabelCopyright
      AutoDeactivate  =   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      Height          =   20
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "© 2026 Mark Gavin"
      TextAlign       =   1
      TextColor       =   &c66666600
      TextFont        =   "System"
      TextSize        =   11.0
      TextUnit        =   0
      Top             =   341
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   360
   End
   Begin Label LabelDescription
      AutoDeactivate  =   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      Height          =   30
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   True
      Scope           =   0
      Selectable      =   False
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Ground station for model rocket flight computer telemetry."
      TextAlign       =   1
      TextColor       =   &c44444400
      TextFont        =   "System"
      TextSize        =   11.0
      TextUnit        =   0
      Top             =   361
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   360
   End
   Begin PushButton ButtonOK
      AutoDeactivate  =   True
      Bold            =   False
      ButtonStyle     =   0
      Cancel          =   True
      Caption         =   "OK"
      Default         =   True
      Enabled         =   True
      Height          =   20
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   130
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   0
      TabIndex        =   5
      TabPanelIndex   =   0
      TabStop         =   True
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   393
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   140
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Open()
		  // Set version from app info
		  LabelVersion.Text = "Version " + Str(App.MajorVersion) + "." + Str(App.MinorVersion)

		  // Load the icon image
		  LoadIconImage
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub LoadIconImage()
		  // Try to load icon image from various locations
		  // Use 512pt files - prefer @2x version for better quality on HiDPI/Retina displays
		  Var theFile As FolderItem
		  Var theFile2x As FolderItem

		  #If TargetMacOS Then
		    // macOS: Try Resources folder in app bundle
		    Var theResources As FolderItem = App.ExecutableFile.Parent.Parent.Child("Resources")
		    If theResources <> Nil And theResources.Exists Then
		      // Look for 512pt files (which are copied by build automation)
		      theFile2x = theResources.Child("ModelRocketAvionic512pt@2x.png")
		      theFile = theResources.Child("ModelRocketAvionic512pt.png")

		      // Prefer @2x for sharper rendering
		      If theFile2x <> Nil And theFile2x.Exists Then
		        pIconImage = Picture.Open(theFile2x)
		        CanvasIcon.Invalidate
		        Return
		      ElseIf theFile <> Nil And theFile.Exists Then
		        pIconImage = Picture.Open(theFile)
		        CanvasIcon.Invalidate
		        Return
		      End If
		    End If

		    // Debug mode: Try artwork directory
		    #If DebugBuild Then
		      Var theSourceFile As FolderItem
		      Var theSourceFile2x As FolderItem
		      theSourceFile2x = New FolderItem("/Users/markgavin/mg/rocket_avionics/artwork/ModelRocketAvionic512pt@2x.png", FolderItem.PathModes.Native)
		      theSourceFile = New FolderItem("/Users/markgavin/mg/rocket_avionics/artwork/ModelRocketAvionic512pt.png", FolderItem.PathModes.Native)

		      If theSourceFile2x <> Nil And theSourceFile2x.Exists Then
		        pIconImage = Picture.Open(theSourceFile2x)
		        CanvasIcon.Invalidate
		        Return
		      ElseIf theSourceFile <> Nil And theSourceFile.Exists Then
		        pIconImage = Picture.Open(theSourceFile)
		        CanvasIcon.Invalidate
		        Return
		      End If
		    #EndIf
		  #Else
		    // Windows/Linux: Look in app directory
		    theFile = App.ExecutableFile.Parent.Child("ModelRocketAvionic512pt.png")
		    If theFile <> Nil And theFile.Exists Then
		      pIconImage = Picture.Open(theFile)
		      CanvasIcon.Invalidate
		      Return
		    End If
		  #EndIf
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pIconImage As Picture
	#tag EndProperty


#tag EndWindowCode

#tag Events CanvasIcon
	#tag Event
		Sub Paint(g As Graphics, areas() As REALbasic.Rect)
		  #Pragma Unused areas

		  // Draw the icon image
		  If pIconImage <> Nil Then
		    g.DrawPicture(pIconImage, 0, 0, g.Width, g.Height, 0, 0, pIconImage.Width, pIconImage.Height)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonOK
	#tag Event
		Sub Action()
		  Self.Close
		End Sub
	#tag EndEvent
#tag EndEvents
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
		Name="Interfaces"
		Visible=true
		Group="ID"
		InitialValue=""
		Type="String"
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
		Name="Width"
		Visible=true
		Group="Size"
		InitialValue="600"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Height"
		Visible=true
		Group="Size"
		InitialValue="400"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MinimumWidth"
		Visible=true
		Group="Size"
		InitialValue="64"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MinimumHeight"
		Visible=true
		Group="Size"
		InitialValue="64"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MaximumWidth"
		Visible=true
		Group="Size"
		InitialValue="32000"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MaximumHeight"
		Visible=true
		Group="Size"
		InitialValue="32000"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Type"
		Visible=true
		Group="Frame"
		InitialValue="0"
		Type="Types"
		EditorType="Enum"
		#tag EnumValues
			"0 - Document"
			"1 - Movable Modal"
			"2 - Modal Dialog"
			"3 - Floating Window"
			"4 - Plain Box"
			"5 - Shadowed Box"
			"6 - Rounded Window"
			"7 - Global Floating Window"
			"8 - Sheet Window"
			"9 - Metal Window"
			"11 - Modeless Dialog"
		#tag EndEnumValues
	#tag EndViewProperty
	#tag ViewProperty
		Name="Title"
		Visible=true
		Group="Frame"
		InitialValue="Untitled"
		Type="String"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasCloseButton"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasMaximizeButton"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasMinimizeButton"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasFullScreenButton"
		Visible=true
		Group="Frame"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Resizeable"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Composite"
		Visible=false
		Group="OS X (Carbon)"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MacProcID"
		Visible=false
		Group="OS X (Carbon)"
		InitialValue="0"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="FullScreen"
		Visible=false
		Group="Behavior"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="ImplicitInstance"
		Visible=true
		Group="Behavior"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="DefaultLocation"
		Visible=true
		Group="Behavior"
		InitialValue="0"
		Type="Locations"
		EditorType="Enum"
		#tag EnumValues
			"0 - Default"
			"1 - Parent Window"
			"2 - Main Screen"
			"3 - Parent Window Screen"
			"4 - Stagger"
		#tag EndEnumValues
	#tag EndViewProperty
	#tag ViewProperty
		Name="Visible"
		Visible=true
		Group="Behavior"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasBackgroundColor"
		Visible=true
		Group="Background"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="BackgroundColor"
		Visible=true
		Group="Background"
		InitialValue="&hFFFFFF"
		Type="Color"
		EditorType="Color"
	#tag EndViewProperty
	#tag ViewProperty
		Name="Backdrop"
		Visible=true
		Group="Background"
		InitialValue=""
		Type="Picture"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MenuBar"
		Visible=true
		Group="Menus"
		InitialValue=""
		Type="MenuBar"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MenuBarVisible"
		Visible=true
		Group="Deprecated"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
#tag EndViewBehavior
