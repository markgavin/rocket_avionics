#tag Window
Begin Window Window_FlightCompare
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   600
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   4294967314
   MenuBarVisible  =   True
   MinHeight       =   600
   MinimizeButton  =   True
   MinWidth        =   900
   Placement       =   0
   Resizeable      =   True
   Title           =   "Compare Flights"
   Visible         =   False
   Width           =   950
   Begin GroupBox GroupFlight1
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Flight 1"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   150
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   280
      Begin Label LabelFlight1Info
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   110
         Index           =   -2147483648
         InitialParent   =   "GroupFlight1"
         Italic          =   False
         Left            =   35
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   True
         Scope           =   2
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "No flight selected.\n\nSelect a flight from History or\ndrag from the flight list."
         TextAlign       =   0
         TextColor       =   &c66666600
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   250
      End
   End
   Begin GroupBox GroupFlight2
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Flight 2"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   150
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   310
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
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
      Width           =   280
      Begin Label LabelFlight2Info
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   110
         Index           =   -2147483648
         InitialParent   =   "GroupFlight2"
         Italic          =   False
         Left            =   325
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   True
         Scope           =   2
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "No flight selected.\n\nSelect a flight from History or\ndrag from the flight list."
         TextAlign       =   0
         TextColor       =   &c66666600
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   250
      End
   End
   Begin GroupBox GroupComparison
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Comparison"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   150
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   600
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   330
      Begin Label LabelComparisonStats
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   110
         Index           =   -2147483648
         InitialParent   =   "GroupComparison"
         Italic          =   False
         Left            =   615
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   True
         Scope           =   2
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Select two flights to see\ncomparison statistics."
         TextAlign       =   0
         TextColor       =   &c66666600
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   300
      End
   End
   Begin GroupBox GroupChart
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Altitude Comparison"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   380
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   180
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   910
      Begin Canvas CanvasCompare
         AllowAutoDeactivate=   True
         AllowFocus      =   False
         AllowFocusRing  =   True
         AllowTabs       =   False
         Backdrop        =   0
         DoubleBuffer    =   False
         Enabled         =   True
         Height          =   340
         Index           =   -2147483648
         InitialParent   =   "GroupChart"
         Left            =   35
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Scope           =   2
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   205
         Transparent     =   True
         Visible         =   True
         Width           =   880
      End
   End
   Begin PushButton ButtonSelectFlight1
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Select Flight 1..."
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   24
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
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   565
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   120
   End
   Begin PushButton ButtonSelectFlight2
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Select Flight 2..."
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   24
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   152
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
      Top             =   565
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   120
   End
   Begin PushButton ButtonSwapFlights
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Swap"
      Default         =   False
      Enabled         =   False
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   24
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   284
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
      Top             =   565
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   80
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
      Height          =   24
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   720
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
      Top             =   565
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
      Height          =   24
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
      TabIndex        =   8
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   565
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin Label LabelLegend
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
      Left            =   376
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   False
      Multiline       =   False
      Scope           =   2
      Selectable      =   False
      TabIndex        =   9
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Blue = Flight 1 | Red = Flight 2"
      TextAlign       =   0
      TextColor       =   &c80808000
      Tooltip         =   ""
      Top             =   565
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   332
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_FlightCompare")
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_FlightCompare")
		  
		  // Check for dark mode
		  #If TargetMacOS Then
		    pIsDarkMode = Color.IsDarkMode
		  #Else
		    pIsDarkMode = False
		  #EndIf
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub DrawComparisonCurve(g As Graphics)
		  // Draw comparison of altitude curves
		  
		  Var theMargin As Integer = 50
		  Var theWidth As Integer = g.Width - theMargin * 2
		  Var theHeight As Integer = g.Height - theMargin * 2
		  
		  // Set colors based on dark mode
		  Var theBackgroundColor As Color
		  Var theGridColor As Color
		  Var theLabelColor As Color
		  Var theAxisColor As Color
		  Var theFlight1Color As Color
		  Var theFlight2Color As Color
		  
		  If pIsDarkMode Then
		    theBackgroundColor = &c1E1E1E
		    theGridColor = &c404040
		    theLabelColor = &cCCCCCC
		    theAxisColor = &c888888
		    theFlight1Color = &c44AAFF  // Brighter blue for dark mode
		    theFlight2Color = &cFF6666   // Brighter red for dark mode
		  Else
		    theBackgroundColor = &cFFFFFF
		    theGridColor = &cE0E0E0
		    theLabelColor = &c000000
		    theAxisColor = &c000000
		    theFlight1Color = &c0066CC
		    theFlight2Color = &cCC0000
		  End If
		  
		  // Draw background
		  g.DrawingColor = theBackgroundColor
		  g.FillRectangle(0, 0, g.Width, g.Height)
		  
		  // Find max values across both flights
		  Var theMaxTime As Double = 0
		  Var theMaxAltitude As Double = 0
		  
		  // Check flight 1
		  If pFlight1 <> Nil Then
		    For Each theSample As TelemetrySample In pFlight1.pSamples
		      Var theTime As Double = theSample.pTimeMs / 1000.0
		      If theTime > theMaxTime Then theMaxTime = theTime
		      If theSample.pAltitudeM > theMaxAltitude Then theMaxAltitude = theSample.pAltitudeM
		    Next
		  End If
		  
		  // Check flight 2
		  If pFlight2 <> Nil Then
		    For Each theSample As TelemetrySample In pFlight2.pSamples
		      Var theTime As Double = theSample.pTimeMs / 1000.0
		      If theTime > theMaxTime Then theMaxTime = theTime
		      If theSample.pAltitudeM > theMaxAltitude Then theMaxAltitude = theSample.pAltitudeM
		    Next
		  End If
		  
		  // Default values if no data
		  If theMaxTime = 0 Then theMaxTime = 60.0
		  If theMaxAltitude = 0 Then theMaxAltitude = 100.0
		  
		  // Add margin to max values
		  theMaxTime = theMaxTime * 1.1
		  theMaxAltitude = theMaxAltitude * 1.1
		  
		  // Round up
		  theMaxAltitude = Ceiling(theMaxAltitude / 50) * 50
		  theMaxTime = Ceiling(theMaxTime * 2) / 2
		  
		  // Draw grid
		  g.DrawingColor = theGridColor
		  For i As Integer = 0 To 10
		    Var theX As Integer = theMargin + (theWidth * i / 10)
		    Var theY As Integer = theMargin + (theHeight * i / 10)
		    g.DrawLine(theX, theMargin, theX, theMargin + theHeight)
		    g.DrawLine(theMargin, theY, theMargin + theWidth, theY)
		  Next
		  
		  // Draw axes
		  g.DrawingColor = theAxisColor
		  g.DrawLine(theMargin, theMargin, theMargin, theMargin + theHeight)
		  g.DrawLine(theMargin, theMargin + theHeight, theMargin + theWidth, theMargin + theHeight)
		  
		  // Draw axis labels
		  g.DrawingColor = theLabelColor
		  g.FontSize = 11
		  g.DrawText("Time (s)", theMargin + theWidth / 2 - 25, g.Height - 10)
		  g.DrawText("Alt (m)", 5, theMargin + 10)
		  
		  // Draw scale values
		  g.FontSize = 10
		  g.DrawText("0", theMargin - 15, theMargin + theHeight + 12)
		  g.DrawText(Format(theMaxTime, "0.0"), theMargin + theWidth - 15, theMargin + theHeight + 12)
		  g.DrawText(Format(theMaxAltitude, "0"), theMargin - 40, theMargin + 5)
		  
		  // Draw flight 1 curve (blue)
		  If pFlight1 <> Nil And pFlight1.pSamples.Count > 1 Then
		    g.DrawingColor = theFlight1Color
		    g.PenSize = 2
		    
		    Var theLastX As Integer = -1
		    Var theLastY As Integer = -1
		    
		    For Each theSample As TelemetrySample In pFlight1.pSamples
		      Var theTime As Double = theSample.pTimeMs / 1000.0
		      Var theX As Integer = theMargin + CType((theTime / theMaxTime) * theWidth, Integer)
		      Var theY As Integer = theMargin + theHeight - CType((theSample.pAltitudeM / theMaxAltitude) * theHeight, Integer)
		      
		      If theLastX >= 0 Then
		        g.DrawLine(theLastX, theLastY, theX, theY)
		      End If
		      
		      theLastX = theX
		      theLastY = theY
		    Next
		  End If
		  
		  // Draw flight 2 curve (red)
		  If pFlight2 <> Nil And pFlight2.pSamples.Count > 1 Then
		    g.DrawingColor = theFlight2Color
		    g.PenSize = 2
		    
		    Var theLastX As Integer = -1
		    Var theLastY As Integer = -1
		    
		    For Each theSample As TelemetrySample In pFlight2.pSamples
		      Var theTime As Double = theSample.pTimeMs / 1000.0
		      Var theX As Integer = theMargin + CType((theTime / theMaxTime) * theWidth, Integer)
		      Var theY As Integer = theMargin + theHeight - CType((theSample.pAltitudeM / theMaxAltitude) * theHeight, Integer)
		      
		      If theLastX >= 0 Then
		        g.DrawLine(theLastX, theLastY, theX, theY)
		      End If
		      
		      theLastX = theX
		      theLastY = theY
		    Next
		  End If
		  
		  g.PenSize = 1
		  
		  // Draw message if no curves
		  If (pFlight1 = Nil Or pFlight1.pSamples.Count < 2) And _
		    (pFlight2 = Nil Or pFlight2.pSamples.Count < 2) Then
		    g.DrawingColor = theLabelColor
		    g.DrawText("Select flights to compare", g.Width / 2 - 80, g.Height / 2)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SetFlight1(inFlight As FlightData)
		  // Set flight 1 for comparison
		  pFlight1 = inFlight
		  
		  If pFlight1 = Nil Then
		    LabelFlight1Info.Text = "No flight selected."
		  Else
		    Var theInfo As String = ""
		    theInfo = theInfo + "Rocket: " + pFlight1.pRocketName + EndOfLine
		    theInfo = theInfo + "Motor: " + pFlight1.pMotorDesignation + EndOfLine
		    theInfo = theInfo + "Max Alt: " + Format(pFlight1.pMaxAltitudeM, "0.0") + " m" + EndOfLine
		    theInfo = theInfo + "Max Vel: " + Format(pFlight1.pMaxVelocityMps, "0.0") + " m/s" + EndOfLine
		    theInfo = theInfo + "Apogee: " + Format(pFlight1.GetApogeeTimeSeconds, "0.00") + " s"
		    LabelFlight1Info.Text = theInfo
		  End If
		  
		  UpdateComparisonStats
		  CanvasCompare.Refresh
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SetFlight2(inFlight As FlightData)
		  // Set flight 2 for comparison
		  pFlight2 = inFlight
		  
		  If pFlight2 = Nil Then
		    LabelFlight2Info.Text = "No flight selected."
		  Else
		    Var theInfo As String = ""
		    theInfo = theInfo + "Rocket: " + pFlight2.pRocketName + EndOfLine
		    theInfo = theInfo + "Motor: " + pFlight2.pMotorDesignation + EndOfLine
		    theInfo = theInfo + "Max Alt: " + Format(pFlight2.pMaxAltitudeM, "0.0") + " m" + EndOfLine
		    theInfo = theInfo + "Max Vel: " + Format(pFlight2.pMaxVelocityMps, "0.0") + " m/s" + EndOfLine
		    theInfo = theInfo + "Apogee: " + Format(pFlight2.GetApogeeTimeSeconds, "0.00") + " s"
		    LabelFlight2Info.Text = theInfo
		  End If
		  
		  UpdateComparisonStats
		  CanvasCompare.Refresh
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub UpdateComparisonStats()
		  // Update comparison statistics
		  If pFlight1 = Nil Or pFlight2 = Nil Then
		    LabelComparisonStats.Text = "Select two flights to see" + EndOfLine + "comparison statistics."
		    ButtonSwapFlights.Enabled = False
		    ButtonExportPDF.Enabled = False
		    ButtonPrint.Enabled = False
		    Return
		  End If
		  
		  ButtonSwapFlights.Enabled = True
		  ButtonExportPDF.Enabled = True
		  ButtonPrint.Enabled = True
		  
		  // Calculate differences
		  Var theAltDiff As Double = pFlight1.pMaxAltitudeM - pFlight2.pMaxAltitudeM
		  Var theVelDiff As Double = pFlight1.pMaxVelocityMps - pFlight2.pMaxVelocityMps
		  Var theApogeeDiff As Double = pFlight1.GetApogeeTimeSeconds - pFlight2.GetApogeeTimeSeconds
		  
		  Var theAltPct As Double = 0
		  If pFlight2.pMaxAltitudeM > 0 Then
		    theAltPct = (theAltDiff / pFlight2.pMaxAltitudeM) * 100
		  End If
		  
		  Var theStats As String = ""
		  theStats = theStats + "Altitude Difference:" + EndOfLine
		  theStats = theStats + "  " + Format(theAltDiff, "+0.0;-0.0") + " m (" + Format(theAltPct, "+0.0;-0.0") + "%)" + EndOfLine + EndOfLine
		  theStats = theStats + "Velocity Difference:" + EndOfLine
		  theStats = theStats + "  " + Format(theVelDiff, "+0.0;-0.0") + " m/s" + EndOfLine + EndOfLine
		  theStats = theStats + "Apogee Time Difference:" + EndOfLine
		  theStats = theStats + "  " + Format(theApogeeDiff, "+0.00;-0.00") + " s"
		  
		  LabelComparisonStats.Text = theStats
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pFlight1 As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlight2 As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pIsDarkMode As Boolean = False
	#tag EndProperty


#tag EndWindowCode

#tag Events CanvasCompare
	#tag Event
		Sub Paint(g As Graphics, areas() As REALbasic.Rect)
		  #Pragma Unused areas
		  DrawComparisonCurve(g)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSelectFlight1
	#tag Event
		Sub Action()
		  // Show flight history to select flight 1
		  // For now, just show history window
		  Window_History.Show
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSelectFlight2
	#tag Event
		Sub Action()
		  // Show flight history to select flight 2
		  Window_History.Show
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSwapFlights
	#tag Event
		Sub Action()
		  // Swap flight 1 and flight 2
		  Var theTemp As FlightData = pFlight1
		  SetFlight1(pFlight2)
		  SetFlight2(theTemp)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonExportPDF
	#tag Event
		Sub Action()
		  If pFlight1 = Nil Or pFlight2 = Nil Then
		    Return
		  End If
		  
		  Var theFlights() As FlightData
		  theFlights.Add(pFlight1)
		  theFlights.Add(pFlight2)
		  Call Module_PrintReport.ExportMultiPDF(theFlights)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonPrint
	#tag Event
		Sub Action()
		  If pFlight1 = Nil Or pFlight2 = Nil Then
		    Return
		  End If
		  
		  Var theFlights() As FlightData
		  theFlights.Add(pFlight1)
		  theFlights.Add(pFlight2)
		  Module_PrintReport.PrintMultiFlight(theFlights)
		End Sub
	#tag EndEvent
#tag EndEvents
