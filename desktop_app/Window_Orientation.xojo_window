#tag Window
Begin Window Window_Orientation
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   500
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   0
   MenuBarVisible  =   True
   MinHeight       =   400
   MinimizeButton  =   True
   MinWidth        =   700
   Placement       =   0
   Resizeable      =   True
   Title           =   "Flight Computer Orientation"
   Visible         =   False
   Width           =   850
   Begin GroupBox GroupData
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "IMU Data"
      Enabled         =   True
      Height          =   470
      Index           =   -2147483648
      Left            =   10
      LockBottom      =   True
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   0
      Top             =   10
      Visible         =   True
      Width           =   250
      Begin Label LabelPitchTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   0
         Text            =   "Pitch:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   35
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelPitchValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   1
         Text            =   "0.0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   35
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelRollTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   2
         Text            =   "Roll:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   58
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelRollValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   3
         Text            =   "0.0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   58
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelHeadingTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   4
         Text            =   "Heading:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   81
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelHeadingValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   5
         Text            =   "0.0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   81
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelSeparator1
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   2
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   6
         Text            =   ""
         TextAlign       =   0
         TextColor       =   &c00000000
         Top             =   110
         Visible         =   True
         Width           =   220
      End
      Begin Label LabelAccelHeader
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   7
         Text            =   "Accelerometer (g)"
         TextAlign       =   0
         TextColor       =   &c00660000
         TextSize        =   11
         Top             =   120
         Visible         =   True
         Width           =   200
      End
      Begin Label LabelAccelXTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   8
         Text            =   "X:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   143
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelAccelXValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   9
         Text            =   "0.000"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   143
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelAccelYTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   10
         Text            =   "Y:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   166
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelAccelYValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   11
         Text            =   "0.000"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   166
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelAccelZTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   12
         Text            =   "Z:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   189
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelAccelZValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   13
         Text            =   "0.000"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   189
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelAccelMagTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   14
         Text            =   "Total:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   212
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelAccelMagValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   15
         Text            =   "1.000"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   212
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelGyroHeader
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   16
         Text            =   "Gyroscope (deg/s)"
         TextAlign       =   0
         TextColor       =   &c00006600
         TextSize        =   11
         Top             =   245
         Visible         =   True
         Width           =   200
      End
      Begin Label LabelGyroXTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   17
         Text            =   "X:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   268
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelGyroXValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   18
         Text            =   "0.0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   268
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelGyroYTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   19
         Text            =   "Y:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   291
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelGyroYValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   20
         Text            =   "0.0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   291
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelGyroZTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   21
         Text            =   "Z:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   314
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelGyroZValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   22
         Text            =   "0.0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   314
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelMagHeader
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   23
         Text            =   "Magnetometer (mG)"
         TextAlign       =   0
         TextColor       =   &c00000066
         TextSize        =   11
         Top             =   347
         Visible         =   True
         Width           =   200
      End
      Begin Label LabelMagXTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   24
         Text            =   "X:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   370
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelMagXValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   25
         Text            =   "0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   370
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelMagYTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   26
         Text            =   "Y:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   393
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelMagYValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   27
         Text            =   "0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   393
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelMagZTitle
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   28
         Text            =   "Z:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   416
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelMagZValue
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   110
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   29
         Text            =   "0"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   12
         Top             =   416
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelUpdateRate
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupData"
         Left            =   25
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   30
         Text            =   "Update: -- Hz"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   10
         Top             =   452
         Visible         =   True
         Width           =   200
      End
   End
   Begin Canvas CanvasOrientation
      AutoDeactivate  =   True
      Backdrop        =   0
      DoubleBuffer    =   True
      Enabled         =   True
      Height          =   470
      Index           =   -2147483648
      Left            =   270
      LockBottom      =   True
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   1
      Top             =   10
      Visible         =   True
      Width           =   570
   End
   Begin Timer TimerRefresh
      Enabled         =   True
      Index           =   -2147483648
      LockedInPosition=   False
      Period          =   50
      RunMode         =   2
      Scope           =   2
      TabPanelIndex   =   0
   End
   Begin Timer TimerKeepAlive
      Enabled         =   False
      Index           =   -2147483648
      LockedInPosition=   False
      Period          =   10000
      RunMode         =   2
      Scope           =   2
      TabPanelIndex   =   0
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_Orientation")

		  pLastUpdateTime = System.Microseconds

		  // Apply dark mode appearance
		  ApplyAppearance()
		End Sub
	#tag EndEvent

	#tag Event
		Sub Activate()
		  // Window is being shown/activated - enable orientation mode
		  // This fires when window becomes visible, not just when created implicitly
		  If Not sOrientationModeActive Then
		    sOrientationModeActive = True
		    If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		      Window_Main.pConnection.SendOrientationMode(True)
		    End If
		    // Start keep-alive timer to prevent flight computer timeout
		    TimerKeepAlive.Enabled = True
		  End If
		End Sub
	#tag EndEvent

	#tag Event
		Sub Deactivate()
		  // Note: Don't disable here - only disable on Close
		End Sub
	#tag EndEvent

	#tag Event
		Sub Close()
		  // Stop keep-alive timer
		  TimerKeepAlive.Enabled = False

		  // Disable high-rate telemetry when closing
		  sOrientationModeActive = False
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendOrientationMode(False)
		  End If

		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_Orientation")
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub UpdateFromTelemetry(inSample As TelemetrySample)
		  // Called by Window_Main when telemetry is received
		  // This avoids the need for duplicate event handlers

		  // Store the latest sample data
		  pPitch = inSample.pPitch
		  pRoll = inSample.pRoll
		  pHeading = inSample.pHeading

		  pAccelX = inSample.pAccelX / 1000.0  // Convert from milli-g to g
		  pAccelY = inSample.pAccelY / 1000.0
		  pAccelZ = inSample.pAccelZ / 1000.0

		  pGyroX = inSample.pGyroX  // Already in dps from gateway
		  pGyroY = inSample.pGyroY
		  pGyroZ = inSample.pGyroZ

		  pMagX = inSample.pMagX
		  pMagY = inSample.pMagY
		  pMagZ = inSample.pMagZ

		  // Calculate total acceleration magnitude
		  pAccelMag = Sqrt(pAccelX * pAccelX + pAccelY * pAccelY + pAccelZ * pAccelZ)

		  // Calculate update rate
		  Var theNow As Double = System.Microseconds
		  Var theDelta As Double = (theNow - pLastUpdateTime) / 1000000.0
		  If theDelta > 0 Then
		    pUpdateRate = 1.0 / theDelta
		  End If
		  pLastUpdateTime = theNow

		  pDataUpdated = True
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub UpdateLabels()
		  // Update data labels
		  LabelPitchValue.Text = Format(pPitch, "+0.0") + Chr(176)
		  LabelRollValue.Text = Format(pRoll, "+0.0") + Chr(176)
		  LabelHeadingValue.Text = Format(pHeading, "0.0") + Chr(176)

		  LabelAccelXValue.Text = Format(pAccelX, "+0.000")
		  LabelAccelYValue.Text = Format(pAccelY, "+0.000")
		  LabelAccelZValue.Text = Format(pAccelZ, "+0.000")
		  LabelAccelMagValue.Text = Format(pAccelMag, "0.000") + " g"

		  LabelGyroXValue.Text = Format(pGyroX, "+0.0")
		  LabelGyroYValue.Text = Format(pGyroY, "+0.0")
		  LabelGyroZValue.Text = Format(pGyroZ, "+0.0")

		  LabelMagXValue.Text = Format(pMagX, "+0")
		  LabelMagYValue.Text = Format(pMagY, "+0")
		  LabelMagZValue.Text = Format(pMagZ, "+0")

		  LabelUpdateRate.Text = "Update: " + Format(pUpdateRate, "0.0") + " Hz"
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawRocket(g As Graphics)
		  // Draw a 3D-style rocket that rotates based on pitch and roll

		  Var theCenterX As Double = g.Width / 2
		  Var theCenterY As Double = g.Height / 2

		  // Background
		  If pIsDarkMode Then
		    g.DrawingColor = &c2D2D2D
		  Else
		    g.DrawingColor = &cF0F0F0
		  End If
		  g.FillRectangle(0, 0, g.Width, g.Height)

		  // Draw horizon reference
		  DrawHorizon(g, theCenterX, theCenterY)

		  // Draw rocket body
		  DrawRocketBody(g, theCenterX, theCenterY)

		  // Draw compass at bottom
		  DrawCompass(g, g.Width - 80, g.Height - 80, 60)

		  // Draw acceleration bar on the right
		  DrawAccelBar(g, g.Width - 40, 50, 20, g.Height - 180)

		  // Draw info text
		  If pIsDarkMode Then
		    g.DrawingColor = &cCCCCCC
		  Else
		    g.DrawingColor = &c333333
		  End If
		  g.FontSize = 14
		  g.Bold = True
		  g.DrawText("Pitch: " + Format(pPitch, "+0.0") + Chr(176), 20, 30)
		  g.DrawText("Roll: " + Format(pRoll, "+0.0") + Chr(176), 20, 50)
		  g.DrawText("Heading: " + Format(pHeading, "0.0") + Chr(176), 20, 70)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawHorizon(g As Graphics, centerX As Double, centerY As Double)
		  // Draw artificial horizon background
		  Var theRadius As Double = Min(g.Width, g.Height) * 0.35

		  // Calculate horizon line position based on pitch
		  Var thePitchOffset As Double = (pPitch / 90.0) * theRadius

		  // Clip to circle
		  g.DrawingColor = &c87CEEB  // Sky blue
		  g.FillOval(centerX - theRadius, centerY - theRadius, theRadius * 2, theRadius * 2)

		  // Draw ground (brown) - rotated by roll
		  Var theRollRad As Double = pRoll * 3.14159 / 180.0

		  // Draw horizon line
		  g.DrawingColor = &c8B4513  // Saddle brown for ground

		  // Create points for the ground half
		  Var theLineLen As Double = theRadius * 1.5
		  Var theCosRoll As Double = Cos(theRollRad)
		  Var theSinRoll As Double = Sin(theRollRad)

		  // Horizon line endpoints
		  Var theX1 As Double = centerX - theLineLen * theCosRoll
		  Var theY1 As Double = centerY - theLineLen * theSinRoll + thePitchOffset
		  Var theX2 As Double = centerX + theLineLen * theCosRoll
		  Var theY2 As Double = centerY + theLineLen * theSinRoll + thePitchOffset

		  // Fill ground below horizon
		  // Approximate with a polygon
		  Var theGroundY As Double = centerY + thePitchOffset
		  If theGroundY < centerY + theRadius Then
		    g.FillRectangle(centerX - theRadius, theGroundY, theRadius * 2, theRadius * 2)
		  End If

		  // Draw horizon line
		  g.DrawingColor = &cFFFFFF
		  g.PenSize = 3
		  g.DrawLine(theX1, theY1, theX2, theY2)
		  g.PenSize = 1

		  // Draw circle outline
		  If pIsDarkMode Then
		    g.DrawingColor = &cCCCCCC
		  Else
		    g.DrawingColor = &c333333
		  End If
		  g.PenSize = 2
		  g.DrawOval(centerX - theRadius, centerY - theRadius, theRadius * 2, theRadius * 2)
		  g.PenSize = 1

		  // Draw center crosshair (fixed reference)
		  g.DrawingColor = &cFFFF00
		  g.PenSize = 2
		  Var theCrossSize As Double = 15
		  g.DrawLine(centerX - theCrossSize, centerY, centerX + theCrossSize, centerY)
		  g.DrawLine(centerX, centerY - theCrossSize, centerX, centerY + theCrossSize)

		  // Draw pitch marks
		  g.DrawingColor = &cFFFFFF
		  g.FontSize = 10
		  For i As Integer = -60 To 60 Step 20
		    If i = 0 Then Continue
		    Var theMarkY As Double = centerY + (i / 90.0) * theRadius + thePitchOffset
		    Var theMarkLen As Double = 20
		    If Abs(theMarkY - centerY) < theRadius Then
		      g.DrawLine(centerX - theMarkLen, theMarkY, centerX + theMarkLen, theMarkY)
		      g.DrawText(Str(i), centerX + theMarkLen + 5, theMarkY + 4)
		    End If
		  Next
		  g.PenSize = 1
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawRocketBody(g As Graphics, centerX As Double, centerY As Double)
		  // Draw a 3D wireframe rocket that rotates with pitch and roll
		  // Uses simple 3D to 2D projection

		  Var thePitchRad As Double = pPitch * 3.14159 / 180.0
		  Var theRollRad As Double = pRoll * 3.14159 / 180.0

		  // Precompute trig values
		  Var theCosPitch As Double = Cos(thePitchRad)
		  Var theSinPitch As Double = Sin(thePitchRad)
		  Var theCosRoll As Double = Cos(theRollRad)
		  Var theSinRoll As Double = Sin(theRollRad)

		  // Rocket dimensions (in 3D space, Y is up/rocket axis)
		  Var theBodyRadius As Double = 12
		  Var theBodyLength As Double = 100
		  Var theNoseLength As Double = 30
		  Var theFinLength As Double = 25
		  Var theFinWidth As Double = 20
		  Var theScale As Double = 1.2

		  // Number of sides for the cylinder (must be Const for array size)
		  Const kNumSides As Integer = 8

		  // Arrays to hold projected points
		  Var theBodyTopX(kNumSides - 1) As Double
		  Var theBodyTopY(kNumSides - 1) As Double
		  Var theBodyBotX(kNumSides - 1) As Double
		  Var theBodyBotY(kNumSides - 1) As Double

		  // Generate cylinder points and project to 2D
		  For i As Integer = 0 To kNumSides - 1
		    Var theAngle As Double = i * 2 * 3.14159 / kNumSides

		    // 3D point on top ring of cylinder (at nose base)
		    Var theX3D As Double = theBodyRadius * Cos(theAngle)
		    Var theY3D As Double = -theBodyLength / 2  // Top of body
		    Var theZ3D As Double = theBodyRadius * Sin(theAngle)

		    // Step 1: Roll around Y axis (rocket's longitudinal axis)
		    Var theX3Dr As Double = theX3D * theCosRoll + theZ3D * theSinRoll
		    Var theZ3Dr As Double = -theX3D * theSinRoll + theZ3D * theCosRoll

		    // Step 2: Pitch around X axis (tips the rolled rocket forward/back)
		    Var theY3Dp As Double = theY3D * theCosPitch - theZ3Dr * theSinPitch
		    Var theZ3Dp As Double = theY3D * theSinPitch + theZ3Dr * theCosPitch

		    // Project to 2D (simple orthographic: X->screen X, Y->screen Y)
		    theBodyTopX(i) = centerX + theX3Dr * theScale
		    theBodyTopY(i) = centerY + theY3Dp * theScale

		    // 3D point on bottom ring of cylinder
		    theY3D = theBodyLength / 2  // Bottom of body

		    // Step 1: Roll (same X3Dr, Z3Dr from above since X and Z unchanged)
		    // Step 2: Pitch
		    theY3Dp = theY3D * theCosPitch - theZ3Dr * theSinPitch
		    theZ3Dp = theY3D * theSinPitch + theZ3Dr * theCosPitch

		    theBodyBotX(i) = centerX + theX3Dr * theScale
		    theBodyBotY(i) = centerY + theY3Dp * theScale
		  Next

		  // Nose tip point (on Y axis, so X=0, Z=0 - roll has no effect)
		  Var theNoseY3D As Double = -theBodyLength / 2 - theNoseLength
		  // Roll doesn't affect points on Y axis (X=0, Z=0)
		  // Pitch around X: Y' = Y * cosPitch, Z' = Y * sinPitch
		  Var theNoseY3Dp As Double = theNoseY3D * theCosPitch
		  Var theNoseX2D As Double = centerX  // X stays at center
		  Var theNoseY2D As Double = centerY + theNoseY3Dp * theScale

		  // Draw the rocket body (cylinder wireframe)
		  g.DrawingColor = &cCC0000  // Red
		  g.PenSize = 2

		  // Draw vertical lines of cylinder
		  For i As Integer = 0 To kNumSides - 1
		    g.DrawLine(theBodyTopX(i), theBodyTopY(i), theBodyBotX(i), theBodyBotY(i))
		  Next

		  // Draw top ring
		  For i As Integer = 0 To kNumSides - 1
		    Var theNext As Integer = (i + 1) Mod kNumSides
		    g.DrawLine(theBodyTopX(i), theBodyTopY(i), theBodyTopX(theNext), theBodyTopY(theNext))
		  Next

		  // Draw bottom ring
		  For i As Integer = 0 To kNumSides - 1
		    Var theNext As Integer = (i + 1) Mod kNumSides
		    g.DrawLine(theBodyBotX(i), theBodyBotY(i), theBodyBotX(theNext), theBodyBotY(theNext))
		  Next

		  // Draw nose cone lines
		  g.DrawingColor = &cFF6600  // Orange nose
		  For i As Integer = 0 To kNumSides - 1 Step 2
		    g.DrawLine(theBodyTopX(i), theBodyTopY(i), theNoseX2D, theNoseY2D)
		  Next

		  // Draw 4 fins at 90-degree intervals
		  g.DrawingColor = &c0066CC  // Blue fins
		  For finNum As Integer = 0 To 3
		    Var theFinAngle As Double = finNum * 3.14159 / 2  // 0, 90, 180, 270 degrees

		    // Fin base point (on body)
		    Var theFinBaseX3D As Double = theBodyRadius * Cos(theFinAngle)
		    Var theFinBaseY3D As Double = theBodyLength / 2 - theFinLength
		    Var theFinBaseZ3D As Double = theBodyRadius * Sin(theFinAngle)

		    // Fin tip point (extends outward)
		    Var theFinTipX3D As Double = (theBodyRadius + theFinWidth) * Cos(theFinAngle)
		    Var theFinTipY3D As Double = theBodyLength / 2
		    Var theFinTipZ3D As Double = (theBodyRadius + theFinWidth) * Sin(theFinAngle)

		    // Fin trailing edge (at body bottom)
		    Var theFinTrailX3D As Double = theBodyRadius * Cos(theFinAngle)
		    Var theFinTrailY3D As Double = theBodyLength / 2
		    Var theFinTrailZ3D As Double = theBodyRadius * Sin(theFinAngle)

		    // Transform fin base: Roll first, then Pitch
		    // Step 1: Roll around Y
		    Var theX3Dr As Double = theFinBaseX3D * theCosRoll + theFinBaseZ3D * theSinRoll
		    Var theZ3Dr As Double = -theFinBaseX3D * theSinRoll + theFinBaseZ3D * theCosRoll
		    // Step 2: Pitch around X
		    Var theY3Dp As Double = theFinBaseY3D * theCosPitch - theZ3Dr * theSinPitch
		    Var theFinBaseX2D As Double = centerX + theX3Dr * theScale
		    Var theFinBaseY2D As Double = centerY + theY3Dp * theScale

		    // Transform fin tip: Roll first, then Pitch
		    theX3Dr = theFinTipX3D * theCosRoll + theFinTipZ3D * theSinRoll
		    theZ3Dr = -theFinTipX3D * theSinRoll + theFinTipZ3D * theCosRoll
		    theY3Dp = theFinTipY3D * theCosPitch - theZ3Dr * theSinPitch
		    Var theFinTipX2D As Double = centerX + theX3Dr * theScale
		    Var theFinTipY2D As Double = centerY + theY3Dp * theScale

		    // Transform fin trail: Roll first, then Pitch
		    theX3Dr = theFinTrailX3D * theCosRoll + theFinTrailZ3D * theSinRoll
		    theZ3Dr = -theFinTrailX3D * theSinRoll + theFinTrailZ3D * theCosRoll
		    theY3Dp = theFinTrailY3D * theCosPitch - theZ3Dr * theSinPitch
		    Var theFinTrailX2D As Double = centerX + theX3Dr * theScale
		    Var theFinTrailY2D As Double = centerY + theY3Dp * theScale

		    // Draw fin triangle
		    g.DrawLine(theFinBaseX2D, theFinBaseY2D, theFinTipX2D, theFinTipY2D)
		    g.DrawLine(theFinTipX2D, theFinTipY2D, theFinTrailX2D, theFinTrailY2D)
		    g.DrawLine(theFinTrailX2D, theFinTrailY2D, theFinBaseX2D, theFinBaseY2D)
		  Next

		  g.PenSize = 1
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawCompass(g As Graphics, centerX As Double, centerY As Double, radius As Double)
		  // Draw compass showing heading

		  // Background circle
		  If pIsDarkMode Then
		    g.DrawingColor = &c404040
		  Else
		    g.DrawingColor = &cFFFFFF
		  End If
		  g.FillOval(centerX - radius, centerY - radius, radius * 2, radius * 2)

		  If pIsDarkMode Then
		    g.DrawingColor = &cCCCCCC
		  Else
		    g.DrawingColor = &c333333
		  End If
		  g.PenSize = 2
		  g.DrawOval(centerX - radius, centerY - radius, radius * 2, radius * 2)

		  // Cardinal directions
		  g.FontSize = 12
		  g.Bold = True
		  If pIsDarkMode Then
		    g.DrawingColor = &cFF6666
		  Else
		    g.DrawingColor = &cCC0000
		  End If
		  g.DrawText("N", centerX - 5, centerY - radius + 15)
		  If pIsDarkMode Then
		    g.DrawingColor = &cCCCCCC
		  Else
		    g.DrawingColor = &c333333
		  End If
		  g.DrawText("E", centerX + radius - 12, centerY + 5)
		  g.DrawText("S", centerX - 4, centerY + radius - 5)
		  g.DrawText("W", centerX - radius + 3, centerY + 5)

		  // Heading arrow
		  Var theHeadingRad As Double = pHeading * 3.14159 / 180.0
		  Var theArrowLen As Double = radius * 0.7

		  Var theArrowX As Double = centerX + theArrowLen * Sin(theHeadingRad)
		  Var theArrowY As Double = centerY - theArrowLen * Cos(theHeadingRad)

		  If pIsDarkMode Then
		    g.DrawingColor = &cFF6666
		  Else
		    g.DrawingColor = &cCC0000
		  End If
		  g.PenSize = 3
		  g.DrawLine(centerX, centerY, theArrowX, theArrowY)

		  // Arrowhead
		  Var theHeadLen As Double = 10
		  Var theA1 As Double = theHeadingRad - 0.4
		  Var theA2 As Double = theHeadingRad + 0.4
		  g.DrawLine(theArrowX, theArrowY, theArrowX - theHeadLen * Sin(theA1), theArrowY + theHeadLen * Cos(theA1))
		  g.DrawLine(theArrowX, theArrowY, theArrowX - theHeadLen * Sin(theA2), theArrowY + theHeadLen * Cos(theA2))

		  g.PenSize = 1
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawAccelBar(g As Graphics, x As Double, y As Double, width As Double, height As Double)
		  // Draw acceleration magnitude bar

		  // Background
		  If pIsDarkMode Then
		    g.DrawingColor = &c404040
		  Else
		    g.DrawingColor = &cFFFFFF
		  End If
		  g.FillRectangle(x, y, width, height)

		  // Outline
		  If pIsDarkMode Then
		    g.DrawingColor = &cCCCCCC
		  Else
		    g.DrawingColor = &c333333
		  End If
		  g.PenSize = 1
		  g.DrawRectangle(x, y, width, height)

		  // Fill based on acceleration (0-4g scale)
		  Var theNorm As Double = Min(pAccelMag / 4.0, 1.0)
		  Var theFillH As Double = theNorm * height

		  // Color gradient based on G (brighter in dark mode)
		  If pAccelMag < 1.5 Then
		    g.DrawingColor = If(pIsDarkMode, &c44FF66, &c00CC00)  // Green
		  ElseIf pAccelMag < 3.0 Then
		    g.DrawingColor = If(pIsDarkMode, &cFFCC44, &cFFAA00)  // Orange
		  Else
		    g.DrawingColor = If(pIsDarkMode, &cFF6666, &cCC0000)  // Red
		  End If

		  g.FillRectangle(x + 1, y + height - theFillH, width - 2, theFillH)

		  // Scale marks
		  If pIsDarkMode Then
		    g.DrawingColor = &cAAAAAA
		  Else
		    g.DrawingColor = &c666666
		  End If
		  g.FontSize = 9
		  For i As Integer = 0 To 4
		    Var theMarkY As Double = y + height - (i / 4.0) * height
		    g.DrawLine(x - 5, theMarkY, x, theMarkY)
		    g.DrawText(Str(i) + "g", x - 25, theMarkY + 4)
		  Next

		  // Label
		  g.FontSize = 10
		  g.Bold = True
		  g.DrawText("Accel", x - 10, y - 10)
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pPitch As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pRoll As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pHeading As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pAccelX As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pAccelY As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pAccelZ As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pAccelMag As Double = 1.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pGyroX As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pGyroY As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pGyroZ As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pMagX As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pMagY As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pMagZ As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDataUpdated As Boolean = False
	#tag EndProperty


	#tag Property, Flags = &h21
		Private pUpdateRate As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastUpdateTime As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		Shared sOrientationModeActive As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pIsDarkMode As Boolean = False
	#tag EndProperty


	#tag Method, Flags = &h21
		Private Sub ApplyAppearance()
		  // Check if dark mode is enabled (macOS only)
		  #If TargetMacOS Then
		    pIsDarkMode = Color.IsDarkMode
		  #Else
		    pIsDarkMode = False
		  #EndIf

		  CanvasOrientation.Invalidate()
		End Sub
	#tag EndMethod


#tag EndWindowCode

#tag Events CanvasOrientation
	#tag Event
		Sub Paint(g As Graphics, areas() As Rect)
		  #Pragma Unused areas

		  DrawRocket(g)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events TimerRefresh
	#tag Event
		Sub Action()
		  // Update display at regular intervals
		  If pDataUpdated Then
		    UpdateLabels()
		    CanvasOrientation.Invalidate()
		    pDataUpdated = False
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events TimerKeepAlive
	#tag Event
		Sub Action()
		  // Send keep-alive every 10 seconds to reset the 30-second timeout on flight computer
		  // This makes the timeout "30 seconds of inactivity" rather than "30 seconds total"
		  If sOrientationModeActive And Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendOrientationMode(True)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents

