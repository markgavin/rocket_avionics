#tag Window
Begin Window Window_Splash
   BackColor       =   &c000000FF
   Backdrop        =   0
   CloseButton     =   False
   Composite       =   False
   Frame           =   4
   FullScreen      =   False
   HasBackColor    =   True
   HasFullScreenButton=   False
   Height          =   512
   ImplicitInstance=   True
   LiveResize      =   "False"
   MacProcID       =   0
   MaxHeight       =   512
   MaximizeButton  =   False
   MaxWidth        =   512
   MenuBar         =   0
   MenuBarVisible  =   False
   MinHeight       =   512
   MinimizeButton  =   False
   MinWidth        =   512
   Placement       =   2
   Resizeable      =   False
   Title           =   ""
   Visible         =   True
   Width           =   512
   Begin Canvas CanvasSplash
      AllowAutoDeactivate=   True
      AllowFocus      =   False
      AllowFocusRing  =   True
      AllowTabs       =   False
      Backdrop        =   0
      DoubleBuffer    =   False
      Enabled         =   True
      Height          =   512
      Index           =   -2147483648
      InitialParent   =   ""
      Left            =   0
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   0
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   0
      Transparent     =   True
      Visible         =   True
      Width           =   512
   End
   Begin Timer TimerClose
      Enabled         =   True
      Index           =   -2147483648
      LockedInPosition=   False
      Period          =   3000
      RunMode         =   1
      Scope           =   0
      TabPanelIndex   =   0
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Open()
		  // Load splash image
		  LoadSplashImage

		  // Create circular mask for truly round window
		  CreateCircularMask

		  #If TargetMacOS Then
		    // Make window transparent and add shadow using declares
		    MakeWindowRound
		  #EndIf

		  // Bring splash to front (main window may have been shown as DefaultWindow)
		  BringToFront
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub BringToFront()
		  // Bring this window to the front using macOS declares
		  #If TargetMacOS Then
		    Declare Function NSClassFromString Lib "Foundation" (className As CFStringRef) As Ptr
		    Declare Function sharedApplication Lib "AppKit" Selector "sharedApplication" (target As Ptr) As Ptr
		    Declare Function windowWithWindowNumber Lib "AppKit" Selector "windowWithWindowNumber:" (target As Ptr, windowNum As Integer) As Ptr
		    Declare Sub makeKeyAndOrderFront Lib "AppKit" Selector "makeKeyAndOrderFront:" (target As Ptr, sender As Ptr)
		    Declare Sub orderFront Lib "AppKit" Selector "orderFront:" (target As Ptr, sender As Ptr)

		    Var theNSApp As Ptr = sharedApplication(NSClassFromString("NSApplication"))
		    Var theNSWindow As Ptr = windowWithWindowNumber(theNSApp, Self.Handle)

		    If theNSWindow <> Nil Then
		      orderFront(theNSWindow, Nil)
		    End If
		  #EndIf
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub CreateCircularMask()
		  // Create a circular window using Core Animation layer with corner radius
		  #If TargetMacOS Then
		    Declare Function NSClassFromString Lib "Foundation" (className As CFStringRef) As Ptr
		    Declare Function sharedApplication Lib "AppKit" Selector "sharedApplication" (target As Ptr) As Ptr
		    Declare Function windowWithWindowNumber Lib "AppKit" Selector "windowWithWindowNumber:" (target As Ptr, windowNum As Integer) As Ptr
		    Declare Function contentView Lib "AppKit" Selector "contentView" (target As Ptr) As Ptr
		    Declare Sub setWantsLayer Lib "AppKit" Selector "setWantsLayer:" (target As Ptr, value As Boolean)
		    Declare Function layer Lib "AppKit" Selector "layer" (target As Ptr) As Ptr
		    Declare Sub setCornerRadius Lib "QuartzCore" Selector "setCornerRadius:" (target As Ptr, radius As Double)
		    Declare Sub setMasksToBounds Lib "QuartzCore" Selector "setMasksToBounds:" (target As Ptr, value As Boolean)

		    Var theNSApp As Ptr = sharedApplication(NSClassFromString("NSApplication"))
		    Var theNSWindow As Ptr = windowWithWindowNumber(theNSApp, Self.Handle)

		    If theNSWindow <> Nil Then
		      Var theContentView As Ptr = contentView(theNSWindow)
		      If theContentView <> Nil Then
		        // Enable layer backing
		        setWantsLayer(theContentView, True)

		        // Get the layer and make it circular
		        Var theLayer As Ptr = layer(theContentView)
		        If theLayer <> Nil Then
		          // Corner radius = half of width for perfect circle
		          setCornerRadius(theLayer, 256.0)
		          setMasksToBounds(theLayer, True)
		        End If
		      End If
		    End If
		  #EndIf
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub LoadSplashImage()
		  // Try to load splash image from various locations
		  // Prefer @2x version for better quality on HiDPI/Retina displays
		  Var theFile As FolderItem
		  Var theFile2x As FolderItem

		  #If TargetMacOS Then
		    // macOS: Try Resources folder in app bundle
		    Var theResources As FolderItem = App.ExecutableFile.Parent.Parent.Child("Resources")
		    If theResources <> Nil And theResources.Exists Then
		      // Try @2x first for better quality on Retina displays
		      theFile2x = theResources.Child("ModelRocketAvionic512pt@2x.png")
		      theFile = theResources.Child("ModelRocketAvionic512pt.png")

		      // Prefer @2x for sharper rendering
		      If theFile2x <> Nil And theFile2x.Exists Then
		        pSplashImage = Picture.Open(theFile2x)
		        Return
		      ElseIf theFile <> Nil And theFile.Exists Then
		        pSplashImage = Picture.Open(theFile)
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
		        pSplashImage = Picture.Open(theSourceFile2x)
		        Return
		      ElseIf theSourceFile <> Nil And theSourceFile.Exists Then
		        pSplashImage = Picture.Open(theSourceFile)
		        Return
		      End If
		    #EndIf
		  #Else
		    // Windows/Linux: Look in app directory
		    theFile = App.ExecutableFile.Parent.Child("ModelRocketAvionic512pt.png")
		    If theFile <> Nil And theFile.Exists Then
		      pSplashImage = Picture.Open(theFile)
		      Return
		    End If
		  #EndIf
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub MakeWindowRound()
		  #If TargetMacOS Then
		    // Get NSWindow from Xojo window handle
		    Declare Function NSClassFromString Lib "Foundation" (className As CFStringRef) As Ptr
		    Declare Function sharedApplication Lib "AppKit" Selector "sharedApplication" (target As Ptr) As Ptr
		    Declare Function windowWithWindowNumber Lib "AppKit" Selector "windowWithWindowNumber:" (target As Ptr, windowNum As Integer) As Ptr
		    Declare Sub setOpaque Lib "AppKit" Selector "setOpaque:" (target As Ptr, value As Boolean)
		    Declare Sub setBackgroundColor Lib "AppKit" Selector "setBackgroundColor:" (target As Ptr, color As Ptr)
		    Declare Sub setHasShadow Lib "AppKit" Selector "setHasShadow:" (target As Ptr, value As Boolean)
		    Declare Function clearColor Lib "AppKit" Selector "clearColor" (target As Ptr) As Ptr

		    Var theNSApp As Ptr = sharedApplication(NSClassFromString("NSApplication"))
		    Var theNSWindow As Ptr = windowWithWindowNumber(theNSApp, Self.Handle)

		    If theNSWindow <> Nil Then
		      // Make window non-opaque (allows transparency)
		      setOpaque(theNSWindow, False)

		      // Set background to clear
		      Var theNSColor As Ptr = NSClassFromString("NSColor")
		      Var theClearColor As Ptr = clearColor(theNSColor)
		      setBackgroundColor(theNSWindow, theClearColor)

		      // Add drop shadow
		      setHasShadow(theNSWindow, True)
		    End If
		  #EndIf
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pSplashImage As Picture
	#tag EndProperty


#tag EndWindowCode

#tag Events CanvasSplash
	#tag Event
		Sub Paint(g As Graphics, areas() As REALbasic.Rect)
		  #Pragma Unused areas

		  // Draw the splash image
		  If pSplashImage <> Nil Then
		    g.DrawPicture(pSplashImage, 0, 0, g.Width, g.Height, 0, 0, pSplashImage.Width, pSplashImage.Height)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events TimerClose
	#tag Event
		Sub Action()
		  // Close splash - main window is already visible behind it
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
