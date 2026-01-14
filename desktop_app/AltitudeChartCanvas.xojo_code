#tag Class
Protected Class AltitudeChartCanvas
Inherits Canvas
	#tag Event
		Sub MouseExit()
		  // Hide tooltip when mouse leaves canvas
		  pShowTooltip = False
		  Self.Refresh
		End Sub
	#tag EndEvent

	#tag Event
		Sub MouseMove(X As Integer, Y As Integer)
		  // Track mouse position and find nearest data point
		  pMouseX = X
		  pMouseY = Y

		  If pFlightData = Nil Or pFlightData.GetSampleCount = 0 Then
		    pShowTooltip = False
		    Return
		  End If

		  // Calculate chart area
		  Var theChartLeft As Integer = pChartMargin + 50
		  Var theChartTop As Integer = pChartMargin + 10
		  Var theChartWidth As Integer = Self.Width - pChartMargin - theChartLeft - 20
		  Var theChartHeight As Integer = Self.Height - pChartMargin - theChartTop - 30

		  // Check if mouse is within chart area
		  If X < theChartLeft Or X > theChartLeft + theChartWidth Or _
		    Y < theChartTop Or Y > theChartTop + theChartHeight Then
		    pShowTooltip = False
		    Self.Refresh
		    Return
		  End If

		  // Calculate scaling factors
		  Var theMaxAltitude As Double = pFlightData.pMaxAltitudeM * 1.1
		  If theMaxAltitude < 10 Then theMaxAltitude = 100
		  Var theMaxTime As Double = pFlightData.pFlightTimeMs / 1000.0 * 1.2
		  If theMaxTime < 1 Then theMaxTime = 60.0

		  Var theXScale As Double = theChartWidth / theMaxTime
		  Var theYScale As Double = theChartHeight / theMaxAltitude

		  // Find nearest data point
		  Var theBestDistance As Double = 999999
		  Var theBestSample As TelemetrySample = Nil

		  For Each theSample As TelemetrySample In pFlightData.pSamples
		    Var theSampleX As Integer = theChartLeft + CType(theSample.GetTimeSeconds * theXScale, Integer)
		    Var theSampleY As Integer = theChartTop + theChartHeight - CType(theSample.pAltitudeM * theYScale, Integer)

		    Var theDist As Double = Sqrt((X - theSampleX) * (X - theSampleX) + (Y - theSampleY) * (Y - theSampleY))
		    If theDist < theBestDistance Then
		      theBestDistance = theDist
		      theBestSample = theSample
		    End If
		  Next

		  If theBestSample <> Nil And theBestDistance < 50 Then
		    pTooltipTime = theBestSample.GetTimeSeconds
		    pTooltipAltitude = theBestSample.pAltitudeM
		    pTooltipVelocity = theBestSample.pVelocityMps
		    pTooltipState = theBestSample.pState
		    pTooltipX = theChartLeft + CType(theBestSample.GetTimeSeconds * theXScale, Integer)
		    pTooltipY = theChartTop + theChartHeight - CType(theBestSample.pAltitudeM * theYScale, Integer)
		    pShowTooltip = True
		  Else
		    pShowTooltip = False
		  End If

		  Self.Refresh
		End Sub
	#tag EndEvent

	#tag Event
		Sub Paint(g As Graphics, areas() As REALbasic.Rect)
		  // Clear background based on dark mode
		  If pDarkMode Then
		    g.ForeColor = &c2D2D2D
		  Else
		    g.ForeColor = Color.White
		  End If
		  g.FillRect(0, 0, g.Width, g.Height)

		  // Calculate chart area
		  Var theChartLeft As Integer = pChartMargin + 50
		  Var theChartTop As Integer = pChartMargin + 10
		  Var theChartWidth As Integer = g.Width - pChartMargin - theChartLeft - 20
		  Var theChartHeight As Integer = g.Height - pChartMargin - theChartTop - 30

		  // Draw grid if enabled
		  If pShowGrid Then
		    DrawGrid(g, theChartLeft, theChartTop, theChartWidth, theChartHeight)
		  End If

		  // Draw axes
		  DrawAxes(g, theChartLeft, theChartTop, theChartWidth, theChartHeight)

		  // Draw data if available
		  If pFlightData <> Nil And pFlightData.GetSampleCount > 0 Then
		    DrawData(g, theChartLeft, theChartTop, theChartWidth, theChartHeight)

		    // Draw apogee marker if enabled
		    If pShowApogee Then
		      DrawApogeeMarker(g, theChartLeft, theChartTop, theChartWidth, theChartHeight)
		    End If
		  Else
		    // No data message
		    If pDarkMode Then
		      g.ForeColor = &c888888
		    Else
		      g.ForeColor = Color.Gray
		    End If
		    g.TextSize = 14
		    Var theMessage As String = "No flight data"
		    g.DrawString(theMessage, (g.Width - g.StringWidth(theMessage)) / 2, g.Height / 2)
		  End If

		  // Draw title
		  DrawTitle(g)

		  // Draw border
		  If pDarkMode Then
		    g.ForeColor = &c555555
		  Else
		    g.ForeColor = &cC0C0C0
		  End If
		  g.PenWidth = 1
		  g.DrawRect(0, 0, g.Width, g.Height)

		  // Draw tooltip if showing
		  If pShowTooltip Then
		    DrawTooltip(g)
		  End If
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub ClearData()
		  // Clear flight data
		  pFlightData = Nil
		  Self.Refresh
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawAxes(g As Graphics, inLeft As Integer, inTop As Integer, inWidth As Integer, inHeight As Integer)
		  // Set axis color based on dark mode
		  If pDarkMode Then
		    g.ForeColor = &cCCCCCC
		  Else
		    g.ForeColor = Color.Black
		  End If
		  g.PenWidth = 2

		  // Y axis
		  g.DrawLine(inLeft, inTop, inLeft, inTop + inHeight)

		  // X axis
		  g.DrawLine(inLeft, inTop + inHeight, inLeft + inWidth, inTop + inHeight)

		  // Axis labels
		  g.TextSize = 11

		  // Y axis label (Altitude m)
		  g.DrawString("Altitude (m)", 5, inTop + inHeight / 2)

		  // X axis label (Time s)
		  g.DrawString("Time (s)", inLeft + inWidth / 2 - 25, inTop + inHeight + 45)

		  // Y axis values
		  g.TextSize = 10
		  Var theMaxAltitude As Double = 100.0
		  If pFlightData <> Nil And pFlightData.pMaxAltitudeM > 0 Then
		    theMaxAltitude = pFlightData.pMaxAltitudeM * 1.1
		  End If

		  Var theYSpacing As Double = inHeight / 5.0
		  For theIndex As Integer = 0 To 5
		    Var theY As Integer = inTop + inHeight - CType(theIndex * theYSpacing, Integer)
		    Var theValue As Double = (theIndex / 5.0) * theMaxAltitude
		    Var theLabel As String = Str(theValue, "0.0")
		    g.DrawString(theLabel, inLeft - g.StringWidth(theLabel) - 5, theY + 4)
		  Next

		  // X axis values (time)
		  Var theMaxTime As Double = 60.0
		  If pFlightData <> Nil And pFlightData.pFlightTimeMs > 0 Then
		    theMaxTime = pFlightData.pFlightTimeMs / 1000.0 * 1.2
		  End If

		  Var theXSpacing As Double = inWidth / 10.0
		  For theIndex As Integer = 0 To 10
		    Var theX As Integer = inLeft + CType(theIndex * theXSpacing, Integer)
		    Var theValue As Double = (theIndex / 10.0) * theMaxTime
		    Var theLabel As String = Str(theValue, "0.0")
		    g.DrawString(theLabel, theX - g.StringWidth(theLabel) / 2, inTop + inHeight + 15)
		  Next
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawData(g As Graphics, inLeft As Integer, inTop As Integer, inWidth As Integer, inHeight As Integer)
		  If pFlightData = Nil Or pFlightData.GetSampleCount < 2 Then
		    Return
		  End If

		  // Calculate scaling factors
		  Var theMaxAltitude As Double = pFlightData.pMaxAltitudeM * 1.1
		  If theMaxAltitude < 10 Then theMaxAltitude = 100

		  Var theMaxTime As Double = pFlightData.pFlightTimeMs / 1000.0 * 1.2
		  If theMaxTime < 1 Then theMaxTime = 60.0

		  Var theXScale As Double = inWidth / theMaxTime
		  Var theYScale As Double = inHeight / theMaxAltitude

		  // Draw altitude curve (brighter blue in dark mode)
		  If pDarkMode Then
		    g.ForeColor = &c44AAFF
		  Else
		    g.ForeColor = &c0066CC
		  End If
		  g.PenWidth = 2

		  Var thePrevX As Integer = -1
		  Var thePrevY As Integer = -1

		  For Each theSample As TelemetrySample In pFlightData.pSamples
		    Var theTimeS As Double = theSample.GetTimeSeconds
		    Var theAltitudeM As Double = theSample.pAltitudeM

		    Var theX As Integer = inLeft + CType(theTimeS * theXScale, Integer)
		    Var theY As Integer = inTop + inHeight - CType(theAltitudeM * theYScale, Integer)

		    If thePrevX >= 0 Then
		      g.DrawLine(thePrevX, thePrevY, theX, theY)
		    End If

		    thePrevX = theX
		    thePrevY = theY
		  Next

		  // Draw velocity curve if enabled
		  If pShowVelocity Then
		    DrawVelocityCurve(g, inLeft, inTop, inWidth, inHeight)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawVelocityCurve(g As Graphics, inLeft As Integer, inTop As Integer, inWidth As Integer, inHeight As Integer)
		  If pFlightData = Nil Or pFlightData.GetSampleCount < 2 Then
		    Return
		  End If

		  // Calculate scaling factors
		  Var theMaxVelocity As Double = pFlightData.pMaxVelocityMps * 1.2
		  If theMaxVelocity < 10 Then theMaxVelocity = 50

		  Var theMaxTime As Double = pFlightData.pFlightTimeMs / 1000.0 * 1.2
		  If theMaxTime < 1 Then theMaxTime = 60.0

		  Var theXScale As Double = inWidth / theMaxTime
		  Var theYScale As Double = inHeight / theMaxVelocity

		  // Draw velocity curve (brighter green in dark mode)
		  If pDarkMode Then
		    g.ForeColor = &c44FF66
		  Else
		    g.ForeColor = &c00AA00
		  End If
		  g.PenWidth = 2

		  Var thePrevX As Integer = -1
		  Var thePrevY As Integer = -1

		  For Each theSample As TelemetrySample In pFlightData.pSamples
		    Var theTimeS As Double = theSample.GetTimeSeconds
		    Var theVelocity As Double = Abs(theSample.pVelocityMps)

		    Var theX As Integer = inLeft + CType(theTimeS * theXScale, Integer)
		    Var theY As Integer = inTop + inHeight - CType(theVelocity * theYScale, Integer)

		    If thePrevX >= 0 Then
		      g.DrawLine(thePrevX, thePrevY, theX, theY)
		    End If

		    thePrevX = theX
		    thePrevY = theY
		  Next
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawGrid(g As Graphics, inLeft As Integer, inTop As Integer, inWidth As Integer, inHeight As Integer)
		  // Grid color based on dark mode
		  If pDarkMode Then
		    g.ForeColor = &c444444
		  Else
		    g.ForeColor = &cE0E0E0
		  End If
		  g.PenWidth = 1

		  // Horizontal grid lines
		  Var theHorizSpacing As Double = inHeight / 5.0
		  For theIndex As Integer = 1 To 4
		    Var theY As Integer = inTop + CType(theIndex * theHorizSpacing, Integer)
		    g.DrawLine(inLeft, theY, inLeft + inWidth, theY)
		  Next

		  // Vertical grid lines
		  Var theVertSpacing As Double = inWidth / 10.0
		  For theIndex As Integer = 1 To 9
		    Var theX As Integer = inLeft + CType(theIndex * theVertSpacing, Integer)
		    g.DrawLine(theX, inTop, theX, inTop + inHeight)
		  Next
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawApogeeMarker(g As Graphics, inLeft As Integer, inTop As Integer, inWidth As Integer, inHeight As Integer)
		  If pFlightData = Nil Or pFlightData.pMaxAltitudeM <= 0 Then
		    Return
		  End If

		  // Find apogee sample
		  Var theApogeeSample As TelemetrySample = Nil
		  For Each theSample As TelemetrySample In pFlightData.pSamples
		    If theSample.pAltitudeM >= pFlightData.pMaxAltitudeM - 0.1 Then
		      theApogeeSample = theSample
		      Exit
		    End If
		  Next

		  If theApogeeSample = Nil Then
		    Return
		  End If

		  // Calculate position
		  Var theMaxAltitude As Double = pFlightData.pMaxAltitudeM * 1.1
		  Var theMaxTime As Double = pFlightData.pFlightTimeMs / 1000.0 * 1.2
		  If theMaxTime < 1 Then theMaxTime = 60.0

		  Var theXScale As Double = inWidth / theMaxTime
		  Var theYScale As Double = inHeight / theMaxAltitude

		  Var theX As Integer = inLeft + CType(theApogeeSample.GetTimeSeconds * theXScale, Integer)
		  Var theY As Integer = inTop + inHeight - CType(theApogeeSample.pAltitudeM * theYScale, Integer)

		  // Draw marker (brighter in dark mode)
		  If pDarkMode Then
		    g.ForeColor = &cFF6644
		  Else
		    g.ForeColor = &cFF3300
		  End If
		  g.PenWidth = 2

		  // Circle marker
		  Var theRadius As Integer = 6
		  g.FillOval(theX - theRadius, theY - theRadius, theRadius * 2, theRadius * 2)

		  // Label
		  g.TextSize = 11
		  If pDarkMode Then
		    g.ForeColor = &cFF8866
		  Else
		    g.ForeColor = &cCC0000
		  End If
		  Var theLabel As String = Format(pFlightData.pMaxAltitudeM, "0.0") + " m"
		  g.DrawString(theLabel, theX + 10, theY - 5)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawTitle(g As Graphics)
		  // Title color based on dark mode
		  If pDarkMode Then
		    g.ForeColor = &cFFFFFF
		  Else
		    g.ForeColor = Color.Black
		  End If
		  g.TextSize = 14
		  g.Bold = True

		  Var theTitle As String = "Flight Altitude Profile"
		  If pFlightData <> Nil And pFlightData.pRocketName <> "" Then
		    theTitle = pFlightData.pRocketName + " Flight Profile"
		  End If

		  g.DrawString(theTitle, (g.Width - g.StringWidth(theTitle)) / 2, 20)
		  g.Bold = False
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawTooltip(g As Graphics)
		  // Draw a tooltip showing time, altitude, velocity, and state
		  g.TextSize = 11
		  g.Bold = False

		  // Calculate tooltip size
		  Var theLineHeight As Integer = 16
		  Var theTooltipWidth As Integer = 140
		  Var theTooltipHeight As Integer = theLineHeight * 4 + 10

		  // Position tooltip near the data point
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
		  If pDarkMode Then
		    g.ForeColor = &c333333
		  Else
		    g.ForeColor = &cFFFFF0
		  End If
		  g.FillRect(theTooltipX, theTooltipY, theTooltipWidth, theTooltipHeight)

		  // Draw tooltip border
		  If pDarkMode Then
		    g.ForeColor = &c666666
		  Else
		    g.ForeColor = &c808080
		  End If
		  g.PenWidth = 1
		  g.DrawRect(theTooltipX, theTooltipY, theTooltipWidth, theTooltipHeight)

		  // Draw tooltip text
		  If pDarkMode Then
		    g.ForeColor = &cFFFFFF
		  Else
		    g.ForeColor = &c000000
		  End If
		  g.DrawString("Time: " + Format(pTooltipTime, "0.00") + " s", theTooltipX + 5, theTooltipY + theLineHeight)
		  g.DrawString("Alt: " + Format(pTooltipAltitude, "0.0") + " m", theTooltipX + 5, theTooltipY + theLineHeight * 2)
		  g.DrawString("Vel: " + Format(pTooltipVelocity, "0.0") + " m/s", theTooltipX + 5, theTooltipY + theLineHeight * 3)
		  g.DrawString("State: " + pTooltipState, theTooltipX + 5, theTooltipY + theLineHeight * 4)

		  // Draw marker circle on the data point
		  g.ForeColor = &cFF6600
		  g.PenWidth = 2
		  g.DrawOval(pTooltipX - 5, pTooltipY - 5, 10, 10)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SetFlightData(inFlightData As FlightData)
		  // Set flight data and refresh chart
		  pFlightData = inFlightData
		  Self.Refresh
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h0
		pChartMargin As Integer = 50
	#tag EndProperty

	#tag Property, Flags = &h0
		pDarkMode As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h0
		pShowGrid As Boolean = True
	#tag EndProperty

	#tag Property, Flags = &h0
		pShowApogee As Boolean = True
	#tag EndProperty

	#tag Property, Flags = &h0
		pShowVelocity As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h0
		pFlightData As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pMouseX As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pMouseY As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pShowTooltip As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipAltitude As Double = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipVelocity As Double = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipTime As Double = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipState As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipX As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTooltipY As Integer = 0
	#tag EndProperty


End Class
#tag EndClass
