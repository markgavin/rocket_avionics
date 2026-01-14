#tag Window
Begin Window Window_History
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   550
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   4294967314
   MenuBarVisible  =   True
   MinHeight       =   550
   MinimizeButton  =   True
   MinWidth        =   900
   Placement       =   0
   Resizeable      =   True
   Title           =   "Flight History"
   Visible         =   False
   Width           =   950
   Begin Listbox ListFlights
      AllowExpandableRows=   False
      AllowFocusRing  =   True
      AllowRowDragging=   False
      AllowRowReordering=   False
      AutoDeactivate  =   True
      AutoHideScrollbars=   True
      Bold            =   False
      Border          =   True
      ColumnCount     =   5
      ColumnsResizable=   True
      ColumnWidths    =   "200,120,100,100,90"
      DefaultRowHeight=   22
      DropIndicatorVisible=   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      GridLinesHorizontal=   0
      GridLinesVertical=   0
      HasHeading      =   True
      HeadingIndex    =   -1
      Height          =   240
      Index           =   -2147483648
      InitialParent   =   ""
      InitialValue    =   "Date/Time	Rocket	Motor	Max Alt (m)	Max Vel (m/s)"
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      RequiresSelection=   False
      Scope           =   2
      ScrollbarHorizontal=   False
      ScrollbarVertical=   True
      SelectionType   =   1
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   620
      _ScrollOffset   =   0
      _ScrollWidth    =   -1
   End
   Begin GroupBox GroupDetails
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Flight Details"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   240
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   652
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   278
      Begin Label LabelPilotId
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   664
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Pilot :"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   60
      End
      Begin Label LabelPilotValue
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   732
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   180
      End
      Begin Label LabelRocketId
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   664
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Rocket :"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   70
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   60
      End
      Begin Label LabelRocketValue
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   732
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   70
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   180
      End
      Begin Label LabelMotorId
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   664
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Motor :"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   95
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   60
      End
      Begin Label LabelMotorValue
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   732
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   95
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   180
      End
      Begin Label LabelLocationId
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   664
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   6
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Location :"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   120
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   65
      End
      Begin Label LabelLocationValue
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   732
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   7
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   120
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   180
      End
      Begin Label LabelNotesId
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   664
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   8
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Notes :"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   145
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   60
      End
      Begin TextArea TextAreaNotes
         AcceptTabs      =   False
         Alignment       =   0
         AllowFocusRing  =   True
         AllowSpellChecking=   True
         AllowStyledText =   True
         AutoDeactivate  =   True
         BackColor       =   &cFFFFFF00
         Bold            =   False
         Border          =   True
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Format          =   ""
         HasHorizontalScrollbar=   False
         Height          =   80
         HideSelection   =   True
         Index           =   -2147483648
         InitialParent   =   "GroupDetails"
         Italic          =   False
         Left            =   664
         LineHeight      =   0.0
         LineSpacing     =   1.0
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         MaximumCharactersAllowed=   0
         Multiline       =   True
         ReadOnly        =   True
         Scope           =   2
         ScrollbarVertical=   True
         TabIndex        =   9
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   ""
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   170
         Transparent     =   False
         Underline       =   False
         UnicodeMode     =   0
         ValidationMask  =   ""
         Visible         =   True
         Width           =   248
      End
   End
   Begin Canvas CanvasChart
      AcceptFocus     =   False
      AcceptTabs      =   False
      AllowFocusRing  =   True
      AutoDeactivate  =   True
      Backdrop        =   0
      DoubleBuffer    =   False
      Enabled         =   True
      Height          =   220
      Index           =   -2147483648
      InitialParent   =   ""
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   272
      Transparent     =   True
      Visible         =   True
      Width           =   910
   End
   Begin PushButton ButtonDelete
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Delete"
      Default         =   False
      Enabled         =   False
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   510
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin PushButton ButtonExport
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Export CSV..."
      Default         =   False
      Enabled         =   False
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   132
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   510
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin PushButton ButtonExportPDF
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Export PDF..."
      Default         =   False
      Enabled         =   False
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   244
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   5
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   510
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin PushButton ButtonPrint
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Print..."
      Default         =   False
      Enabled         =   False
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   356
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   6
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   510
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin PushButton ButtonViewInMain
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "View in Main"
      Default         =   False
      Enabled         =   False
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   830
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   7
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   510
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin Label LabelSelectionHint
      AutoDeactivate  =   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   468
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   False
      Multiline       =   False
      Scope           =   2
      Selectable      =   False
      TabIndex        =   8
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Tip: Select multiple flights (Cmd+Click) to overlay altitude curves"
      TextAlign       =   0
      TextColor       =   &c80808000
      Tooltip         =   ""
      Top             =   510
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   350
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_History")
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_History")
		  
		  // Use the shared database from App
		  pDatabase = App.pDatabase
		  
		  // Check for dark mode
		  #If TargetMacOS Then
		    pIsDarkMode = Color.IsDarkMode
		  #Else
		    pIsDarkMode = False
		  #EndIf
		  
		  // Load flight list
		  RefreshFlightList
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub DrawChart(g As Graphics)
		  // Draw a multi-curve altitude chart with grid
		  
		  // Set colors based on dark mode
		  Var theBackgroundColor As Color
		  Var theGridColor As Color
		  Var theLabelColor As Color
		  Var theAxisColor As Color
		  
		  If pIsDarkMode Then
		    theBackgroundColor = &c1E1E1E
		    theGridColor = &c404040
		    theLabelColor = &cCCCCCC
		    theAxisColor = &c888888
		  Else
		    theBackgroundColor = &cFFFFFF
		    theGridColor = &cE0E0E0
		    theLabelColor = &c666666
		    theAxisColor = &c888888
		  End If
		  
		  // Clear background
		  g.DrawingColor = theBackgroundColor
		  g.FillRectangle(0, 0, g.Width, g.Height)
		  
		  // Check if we have any data to display
		  If pSelectedFlights.Count = 0 Then
		    g.DrawingColor = theLabelColor
		    g.DrawText("No flight selected", g.Width / 2 - 50, g.Height / 2)
		    Return
		  End If
		  
		  Var theMargin As Integer = 50
		  Var theChartWidth As Integer = g.Width - theMargin - 20
		  Var theChartHeight As Integer = g.Height - theMargin - 20
		  
		  // Find max values across all selected flights
		  Var theMaxAltitude As Double = 0
		  Var theMaxTime As Integer = 0
		  
		  For Each theFlight As FlightData In pSelectedFlights
		    If theFlight.pMaxAltitudeM > theMaxAltitude Then
		      theMaxAltitude = theFlight.pMaxAltitudeM
		    End If
		    For Each theSample As TelemetrySample In theFlight.pSamples
		      If theSample.pTimeMs > theMaxTime Then
		        theMaxTime = theSample.pTimeMs
		      End If
		    Next
		  Next
		  
		  If theMaxAltitude <= 0 Or theMaxTime <= 0 Then
		    g.DrawingColor = theLabelColor
		    g.DrawText("No valid data", g.Width / 2 - 35, g.Height / 2)
		    Return
		  End If
		  
		  // Round up max values for nice grid lines
		  theMaxAltitude = Ceiling(theMaxAltitude / 50) * 50
		  theMaxTime = CType(Ceiling(theMaxTime / 5000) * 5000, Integer)
		  
		  // Draw grid
		  g.DrawingColor = theGridColor
		  Var theGridLines As Integer = 5
		  
		  // Horizontal grid lines (altitude)
		  For i As Integer = 0 To theGridLines
		    Var theY As Integer = g.Height - theMargin - CType((i / theGridLines) * theChartHeight, Integer)
		    g.DrawLine(theMargin, theY, g.Width - 20, theY)
		    
		    // Label
		    g.DrawingColor = theLabelColor
		    Var theLabel As String = Format(theMaxAltitude * i / theGridLines, "0")
		    g.DrawText(theLabel, 5, theY + 4)
		    g.DrawingColor = theGridColor
		  Next
		  
		  // Vertical grid lines (time)
		  For i As Integer = 0 To theGridLines
		    Var theX As Integer = theMargin + CType((i / theGridLines) * theChartWidth, Integer)
		    g.DrawLine(theX, 20, theX, g.Height - theMargin)
		    
		    // Label
		    g.DrawingColor = theLabelColor
		    Var theLabel As String = Format(theMaxTime * i / theGridLines / 1000, "0")
		    g.DrawText(theLabel + " s", theX - 10, g.Height - 10)
		    g.DrawingColor = theGridColor
		  Next
		  
		  // Draw axes
		  g.DrawingColor = theAxisColor
		  g.DrawLine(theMargin, 20, theMargin, g.Height - theMargin)
		  g.DrawLine(theMargin, g.Height - theMargin, g.Width - 20, g.Height - theMargin)
		  
		  // Y-axis label
		  g.DrawingColor = theLabelColor
		  g.DrawText("Alt (m)", 5, 15)
		  
		  // Define colors for multiple curves
		  Var theColors() As Color
		  theColors.Add(&c0066FF)  // Blue
		  theColors.Add(&cFF3300)  // Red
		  theColors.Add(&c00CC00)  // Green
		  theColors.Add(&cFF9900)  // Orange
		  theColors.Add(&c9900FF)  // Purple
		  theColors.Add(&c00CCCC)  // Cyan
		  theColors.Add(&cCC00CC)  // Magenta
		  theColors.Add(&c666600)  // Olive
		  
		  // Draw each selected flight curve
		  Var theColorIndex As Integer = 0
		  For Each theFlight As FlightData In pSelectedFlights
		    g.DrawingColor = theColors(theColorIndex Mod theColors.Count)
		    g.PenSize = 2
		    
		    Var theLastX As Integer = -1
		    Var theLastY As Integer = -1
		    
		    For Each theSample As TelemetrySample In theFlight.pSamples
		      Var theX As Integer = theMargin + CType((theSample.pTimeMs / theMaxTime) * theChartWidth, Integer)
		      Var theY As Integer = g.Height - theMargin - CType((theSample.pAltitudeM / theMaxAltitude) * theChartHeight, Integer)
		      
		      If theLastX >= 0 Then
		        g.DrawLine(theLastX, theLastY, theX, theY)
		      End If
		      
		      theLastX = theX
		      theLastY = theY
		    Next
		    
		    theColorIndex = theColorIndex + 1
		  Next
		  
		  g.PenSize = 1
		  
		  // Draw legend if multiple curves
		  If pSelectedFlights.Count > 1 Then
		    Var theLegendY As Integer = 25
		    theColorIndex = 0
		    For Each theFlight As FlightData In pSelectedFlights
		      g.DrawingColor = theColors(theColorIndex Mod theColors.Count)
		      g.FillRectangle(g.Width - 150, theLegendY, 15, 10)
		      g.DrawingColor = theLabelColor
		      Var theRocket As String = theFlight.pRocketName
		      If theRocket = "" Then
		        theRocket = "Flight " + Str(theColorIndex + 1)
		      End If
		      If theRocket.Length > 15 Then
		        theRocket = theRocket.Left(15) + "..."
		      End If
		      g.DrawText(theRocket, g.Width - 130, theLegendY + 9)
		      theLegendY = theLegendY + 15
		      theColorIndex = theColorIndex + 1
		    Next
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawTooltip(g As Graphics)
		  // Draw a tooltip showing time and altitude at the data point
		  Var theColors() As Color
		  theColors.Add(&c0066FF)
		  theColors.Add(&cFF3300)
		  theColors.Add(&c00CC00)
		  theColors.Add(&cFF9900)
		  theColors.Add(&c9900FF)
		  theColors.Add(&c00CCCC)
		  theColors.Add(&cCC00CC)
		  theColors.Add(&c666600)
		  
		  g.FontSize = 11
		  g.Bold = False
		  
		  // Calculate tooltip size
		  Var theLineHeight As Integer = 16
		  Var theTooltipWidth As Integer = 140
		  Var theTooltipHeight As Integer = theLineHeight * 2 + 10
		  
		  // Position tooltip near the data point but offset to avoid cursor
		  Var theTooltipX As Integer = pTooltipX + 15
		  Var theTooltipY As Integer = pTooltipY - theTooltipHeight - 5
		  
		  // Keep tooltip on screen
		  If theTooltipX + theTooltipWidth > g.Width - 5 Then
		    theTooltipX = pTooltipX - theTooltipWidth - 15
		  End If
		  If theTooltipY < 5 Then
		    theTooltipY = pTooltipY + 20
		  End If
		  
		  // Draw tooltip background
		  If pIsDarkMode Then
		    g.DrawingColor = &c333333
		  Else
		    g.DrawingColor = &cFFFFF0
		  End If
		  g.FillRectangle(theTooltipX, theTooltipY, theTooltipWidth, theTooltipHeight)
		  
		  // Draw tooltip border
		  If pIsDarkMode Then
		    g.DrawingColor = &c666666
		  Else
		    g.DrawingColor = &c808080
		  End If
		  g.PenSize = 1
		  g.DrawRectangle(theTooltipX, theTooltipY, theTooltipWidth, theTooltipHeight)
		  
		  // Draw tooltip text
		  If pIsDarkMode Then
		    g.DrawingColor = &cFFFFFF
		  Else
		    g.DrawingColor = &c000000
		  End If
		  g.DrawText("Time: " + Format(pTooltipTime, "0.00") + " s", theTooltipX + 5, theTooltipY + theLineHeight)
		  g.DrawText("Altitude: " + Format(pTooltipAltitude, "0.0") + " m", theTooltipX + 5, theTooltipY + theLineHeight * 2)
		  
		  // Draw marker circle on the data point with the curve's color
		  g.DrawingColor = theColors(pTooltipFlightIndex Mod theColors.Count)
		  g.PenSize = 2
		  g.FillOval(pTooltipX - 5, pTooltipY - 5, 10, 10)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetSelectedFlights() As FlightData()
		  // Return array of selected flights for printing/exporting
		  Return pSelectedFlights
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub RefreshFlightList()
		  // Clear existing rows
		  ListFlights.RemoveAllRows
		  ReDim pFlightIds(-1)
		  
		  // Get flight list from database
		  Var theFlights() As Dictionary = pDatabase.GetFlightList
		  
		  For Each theFlight As Dictionary In theFlights
		    ListFlights.AddRow( _
		    theFlight.Value("timestamp").StringValue, _
		    theFlight.Value("rocket").StringValue, _
		    theFlight.Value("motor").StringValue, _
		    Format(theFlight.Value("max_altitude").DoubleValue, "0.0"), _
		    Format(theFlight.Value("max_velocity").DoubleValue, "0.0"))
		    
		    pFlightIds.Add(theFlight.Value("flight_id").StringValue)
		  Next
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ShowFlightDetails(inFlightId As String)
		  // Load and display flight details
		  pSelectedFlight = pDatabase.LoadFlight(inFlightId)
		  
		  If Not (pSelectedFlight IsA FlightData) Then
		    Return
		  End If
		  
		  // Update labels
		  LabelPilotValue.Text = If(pSelectedFlight.pPilotName = "", "--", pSelectedFlight.pPilotName)
		  LabelRocketValue.Text = If(pSelectedFlight.pRocketName = "", "--", pSelectedFlight.pRocketName)
		  LabelMotorValue.Text = If(pSelectedFlight.pMotorDesignation = "", "--", pSelectedFlight.pMotorDesignation)
		  LabelLocationValue.Text = If(pSelectedFlight.pLocation = "", "--", pSelectedFlight.pLocation)
		  TextAreaNotes.Text = pSelectedFlight.pNotes
		  
		  // Enable buttons
		  ButtonDelete.Enabled = True
		  ButtonExport.Enabled = True
		  ButtonExportPDF.Enabled = True
		  ButtonPrint.Enabled = True
		  ButtonViewInMain.Enabled = True
		  
		  // Refresh chart
		  CanvasChart.Refresh
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pDatabase As FlightDatabase
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlightIds() As String
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pIsDarkMode As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pSelectedFlight As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pSelectedFlights() As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pShowTooltip As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipAltitude As Double = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipFlightIndex As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipTime As Double = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipX As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipY As Integer = 0
	#tag EndProperty


#tag EndWindowCode

#tag Events ListFlights
	#tag Event
		Sub Change()
		  // Collect all selected flights
		  ReDim pSelectedFlights(-1)
		  
		  For theRow As Integer = 0 To Me.RowCount - 1
		    If Me.Selected(theRow) Then
		      Var theFlightData As FlightData = pDatabase.LoadFlight(pFlightIds(theRow))
		      If theFlightData IsA FlightData Then
		        pSelectedFlights.Add(theFlightData)
		      End If
		    End If
		  Next
		  
		  If pSelectedFlights.Count > 0 Then
		    // Show details for the first selected flight
		    pSelectedFlight = pSelectedFlights(0)
		    LabelPilotValue.Text = If(pSelectedFlight.pPilotName = "", "--", pSelectedFlight.pPilotName)
		    LabelRocketValue.Text = If(pSelectedFlight.pRocketName = "", "--", pSelectedFlight.pRocketName)
		    LabelMotorValue.Text = If(pSelectedFlight.pMotorDesignation = "", "--", pSelectedFlight.pMotorDesignation)
		    LabelLocationValue.Text = If(pSelectedFlight.pLocation = "", "--", pSelectedFlight.pLocation)
		    TextAreaNotes.Text = pSelectedFlight.pNotes
		    
		    // Enable buttons (delete/export only for single selection)
		    ButtonDelete.Enabled = (pSelectedFlights.Count = 1)
		    ButtonExport.Enabled = True
		    ButtonExportPDF.Enabled = True
		    ButtonPrint.Enabled = True
		    ButtonViewInMain.Enabled = (pSelectedFlights.Count = 1)
		  Else
		    // No selection - clear details
		    LabelPilotValue.Text = "--"
		    LabelRocketValue.Text = "--"
		    LabelMotorValue.Text = "--"
		    LabelLocationValue.Text = "--"
		    TextAreaNotes.Text = ""
		    pSelectedFlight = Nil
		    ButtonDelete.Enabled = False
		    ButtonExport.Enabled = False
		    ButtonExportPDF.Enabled = False
		    ButtonPrint.Enabled = False
		    ButtonViewInMain.Enabled = False
		  End If
		  
		  CanvasChart.Refresh
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events CanvasChart
	#tag Event
		Sub MouseExit()
		  // Hide tooltip when mouse leaves canvas
		  pShowTooltip = False
		  Me.Refresh
		End Sub
	#tag EndEvent
	#tag Event
		Sub MouseMove(X As Integer, Y As Integer)
		  // Track mouse position and find nearest data point
		  If pSelectedFlights.Count = 0 Then
		    pShowTooltip = False
		    Return
		  End If
		  
		  Var theMargin As Integer = 50
		  Var theChartWidth As Integer = Me.Width - theMargin - 20
		  Var theChartHeight As Integer = Me.Height - theMargin - 20
		  
		  // Check if mouse is within chart area
		  If X < theMargin Or X > Me.Width - 20 Or Y < 20 Or Y > Me.Height - theMargin Then
		    pShowTooltip = False
		    Me.Refresh
		    Return
		  End If
		  
		  // Find max values (same as DrawChart)
		  Var theMaxAltitude As Double = 0
		  Var theMaxTime As Integer = 0
		  For Each theFlight As FlightData In pSelectedFlights
		    If theFlight.pMaxAltitudeM > theMaxAltitude Then
		      theMaxAltitude = theFlight.pMaxAltitudeM
		    End If
		    For Each theSample As TelemetrySample In theFlight.pSamples
		      If theSample.pTimeMs > theMaxTime Then
		        theMaxTime = theSample.pTimeMs
		      End If
		    Next
		  Next
		  
		  If theMaxAltitude <= 0 Or theMaxTime <= 0 Then
		    pShowTooltip = False
		    Return
		  End If
		  
		  theMaxAltitude = Ceiling(theMaxAltitude / 50) * 50
		  theMaxTime = CType(Ceiling(theMaxTime / 5000) * 5000, Integer)
		  
		  // Find nearest data point across all curves
		  Var theBestDistance As Double = 999999
		  Var theBestSample As TelemetrySample = Nil
		  Var theBestFlightIndex As Integer = 0
		  
		  Var theFlightIndex As Integer = 0
		  For Each theFlight As FlightData In pSelectedFlights
		    For Each theSample As TelemetrySample In theFlight.pSamples
		      Var theSampleX As Integer = theMargin + CType((theSample.pTimeMs / theMaxTime) * theChartWidth, Integer)
		      Var theSampleY As Integer = Me.Height - theMargin - CType((theSample.pAltitudeM / theMaxAltitude) * theChartHeight, Integer)
		      
		      Var theDist As Double = Sqrt((X - theSampleX) * (X - theSampleX) + (Y - theSampleY) * (Y - theSampleY))
		      If theDist < theBestDistance Then
		        theBestDistance = theDist
		        theBestSample = theSample
		        theBestFlightIndex = theFlightIndex
		      End If
		    Next
		    theFlightIndex = theFlightIndex + 1
		  Next
		  
		  If theBestSample <> Nil And theBestDistance < 50 Then
		    pTooltipTime = theBestSample.pTimeMs / 1000.0
		    pTooltipAltitude = theBestSample.pAltitudeM
		    pTooltipX = theMargin + CType((theBestSample.pTimeMs / theMaxTime) * theChartWidth, Integer)
		    pTooltipY = Me.Height - theMargin - CType((theBestSample.pAltitudeM / theMaxAltitude) * theChartHeight, Integer)
		    pTooltipFlightIndex = theBestFlightIndex
		    pShowTooltip = True
		  Else
		    pShowTooltip = False
		  End If
		  
		  Me.Refresh
		End Sub
	#tag EndEvent
	#tag Event
		Sub Paint(g As Graphics, areas() As REALbasic.Rect)
		  #Pragma Unused areas
		  
		  // Clear background
		  g.DrawingColor = &cFFFFFF
		  g.FillRectangle(0, 0, g.Width, g.Height)
		  
		  // Draw chart
		  DrawChart(g)
		  
		  // Draw tooltip if showing
		  If pShowTooltip Then
		    DrawTooltip(g)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDelete
	#tag Event
		Sub Action()
		  If Not (pSelectedFlight IsA FlightData) Then
		    Return
		  End If
		  
		  // Confirm deletion
		  Var theResult As Integer = MsgBox("Delete this flight? This cannot be undone.", 1, "Confirm Delete")
		  
		  If theResult = 1 Then
		    If pDatabase.DeleteFlight(pSelectedFlight.pFlightId) Then
		      RefreshFlightList
		      pSelectedFlight = Nil
		      ReDim pSelectedFlights(-1)
		      LabelPilotValue.Text = "--"
		      LabelRocketValue.Text = "--"
		      LabelMotorValue.Text = "--"
		      LabelLocationValue.Text = "--"
		      TextAreaNotes.Text = ""
		      ButtonDelete.Enabled = False
		      ButtonExport.Enabled = False
		      ButtonExportPDF.Enabled = False
		      ButtonPrint.Enabled = False
		      ButtonViewInMain.Enabled = False
		      CanvasChart.Refresh
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonExport
	#tag Event
		Sub Action()
		  If pSelectedFlights.Count = 0 Then
		    Return
		  End If
		  
		  // Show save dialog
		  Var theDlg As New SaveAsDialog
		  theDlg.Title = "Export Flight Data"
		  
		  If pSelectedFlights.Count = 1 Then
		    theDlg.SuggestedFileName = "flight_" + pSelectedFlights(0).pFlightId.Left(8) + ".csv"
		    
		    Var theFile As FolderItem = theDlg.ShowModal
		    If theFile <> Nil Then
		      Var theOutput As TextOutputStream = TextOutputStream.Create(theFile)
		      theOutput.Write(pSelectedFlights(0).ToCSV)
		      theOutput.Close
		    End If
		  Else
		    // Export multiple flights
		    theDlg.SuggestedFileName = "flights_comparison.csv"
		    
		    Var theFile As FolderItem = theDlg.ShowModal
		    If theFile <> Nil Then
		      Var theOutput As TextOutputStream = TextOutputStream.Create(theFile)
		      theOutput.WriteLine("# Flight Comparison Export")
		      theOutput.WriteLine("# Flights: " + Str(pSelectedFlights.Count))
		      theOutput.WriteLine("#")
		      For Each theFlight As FlightData In pSelectedFlights
		        theOutput.Write(theFlight.ToCSV)
		        theOutput.WriteLine("")
		        theOutput.WriteLine("# ---")
		        theOutput.WriteLine("")
		      Next
		      theOutput.Close
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonExportPDF
	#tag Event
		Sub Action()
		  If pSelectedFlights.Count = 0 Then
		    Return
		  End If
		  
		  If pSelectedFlights.Count = 1 Then
		    Call Module_PrintReport.ExportPDF(pSelectedFlights(0))
		  Else
		    Call Module_PrintReport.ExportMultiPDF(pSelectedFlights)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonPrint
	#tag Event
		Sub Action()
		  If pSelectedFlights.Count = 0 Then
		    Return
		  End If
		  
		  If pSelectedFlights.Count = 1 Then
		    Module_PrintReport.PrintFlight(pSelectedFlights(0))
		  Else
		    Module_PrintReport.PrintMultiFlight(pSelectedFlights)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonViewInMain
	#tag Event
		Sub Action()
		  If Not (pSelectedFlight IsA FlightData) Then
		    Return
		  End If
		  
		  // Send flight data to main window for display
		  Window_Main.DisplayFlightData(pSelectedFlight)
		  Window_Main.Show
		End Sub
	#tag EndEvent
#tag EndEvents
