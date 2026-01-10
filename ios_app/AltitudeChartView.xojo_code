#tag Class
Protected Class AltitudeChartView
Inherits MobileCanvas
	#tag Event
		Sub Paint(g As Graphics)
		  // Draw the altitude chart
		  DrawChart(g)
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub SetFlightData(inData As FlightData)
		  // Set flight data and refresh chart
		  pFlightData = inData
		  Self.Invalidate
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub DrawChart(g As Graphics)
		  // Chart margins
		  Const kMarginLeft = 60
		  Const kMarginRight = 20
		  Const kMarginTop = 30
		  Const kMarginBottom = 50

		  Var theWidth As Double = g.Width
		  Var theHeight As Double = g.Height

		  // Chart area
		  Var theChartLeft As Double = kMarginLeft
		  Var theChartRight As Double = theWidth - kMarginRight
		  Var theChartTop As Double = kMarginTop
		  Var theChartBottom As Double = theHeight - kMarginBottom
		  Var theChartWidth As Double = theChartRight - theChartLeft
		  Var theChartHeight As Double = theChartBottom - theChartTop

		  // Background
		  g.DrawingColor = &cF5F5F5
		  g.FillRectangle(0, 0, theWidth, theHeight)

		  // Chart background
		  g.DrawingColor = &cFFFFFF
		  g.FillRectangle(theChartLeft, theChartTop, theChartWidth, theChartHeight)

		  // Chart border
		  g.DrawingColor = &cCCCCCC
		  g.DrawRectangle(theChartLeft, theChartTop, theChartWidth, theChartHeight)

		  // Title
		  g.DrawingColor = &c333333
		  g.FontSize = 16
		  g.Bold = True
		  g.DrawText("Altitude Profile", theChartLeft + (theChartWidth - g.TextWidth("Altitude Profile")) / 2, 22)
		  g.Bold = False

		  // Check for data
		  If pFlightData = Nil Or pFlightData.GetSampleCount < 2 Then
		    // No data message
		    g.DrawingColor = &c999999
		    g.FontSize = 14
		    Var theMessage As String = "Awaiting telemetry data..."
		    g.DrawText(theMessage, theChartLeft + (theChartWidth - g.TextWidth(theMessage)) / 2, theChartTop + theChartHeight / 2)
		    Return
		  End If

		  // Calculate data bounds
		  Var theMaxTime As Double = 0
		  Var theMaxAlt As Double = 0

		  For Each theSample As TelemetrySample In pFlightData.pSamples
		    If theSample.GetTimeSeconds > theMaxTime Then
		      theMaxTime = theSample.GetTimeSeconds
		    End If
		    If theSample.pAltitudeM > theMaxAlt Then
		      theMaxAlt = theSample.pAltitudeM
		    End If
		  Next

		  // Add padding to max values
		  theMaxTime = Max(theMaxTime, 10)
		  theMaxAlt = Max(theMaxAlt * 1.1, 50)

		  // Draw grid lines
		  g.DrawingColor = &cE0E0E0
		  g.FontSize = 10

		  // Horizontal grid (altitude)
		  Var theAltStep As Double = CalculateNiceStep(theMaxAlt, 5)
		  Var theAlt As Double = 0
		  While theAlt <= theMaxAlt
		    Var theY As Double = theChartBottom - (theAlt / theMaxAlt) * theChartHeight
		    g.DrawLine(theChartLeft, theY, theChartRight, theY)

		    g.DrawingColor = &c666666
		    Var theLabel As String = Format(theAlt, "0") + " m"
		    g.DrawText(theLabel, theChartLeft - g.TextWidth(theLabel) - 5, theY + 4)
		    g.DrawingColor = &cE0E0E0

		    theAlt = theAlt + theAltStep
		  Wend

		  // Vertical grid (time)
		  Var theTimeStep As Double = CalculateNiceStep(theMaxTime, 6)
		  Var theTime As Double = 0
		  While theTime <= theMaxTime
		    Var theX As Double = theChartLeft + (theTime / theMaxTime) * theChartWidth
		    g.DrawLine(theX, theChartTop, theX, theChartBottom)

		    g.DrawingColor = &c666666
		    Var theLabel As String = Format(theTime, "0") + "s"
		    g.DrawText(theLabel, theX - g.TextWidth(theLabel) / 2, theChartBottom + 15)
		    g.DrawingColor = &cE0E0E0

		    theTime = theTime + theTimeStep
		  Wend

		  // Draw altitude line
		  g.DrawingColor = &c0066CC
		  Var theLastX As Double = -1
		  Var theLastY As Double = -1

		  For Each theSample As TelemetrySample In pFlightData.pSamples
		    Var theX As Double = theChartLeft + (theSample.GetTimeSeconds / theMaxTime) * theChartWidth
		    Var theY As Double = theChartBottom - (theSample.pAltitudeM / theMaxAlt) * theChartHeight

		    If theLastX >= 0 Then
		      g.DrawLine(theLastX, theLastY, theX, theY)
		    End If

		    theLastX = theX
		    theLastY = theY
		  Next

		  // Draw apogee marker
		  If pFlightData.pMaxAltitudeM > 0 Then
		    Var theApogeeX As Double = theChartLeft + (pFlightData.GetApogeeTimeSeconds / theMaxTime) * theChartWidth
		    Var theApogeeY As Double = theChartBottom - (pFlightData.pMaxAltitudeM / theMaxAlt) * theChartHeight

		    // Apogee circle
		    g.DrawingColor = &cFF6600
		    g.FillOval(theApogeeX - 6, theApogeeY - 6, 12, 12)

		    // Apogee label
		    g.DrawingColor = &cFF6600
		    g.FontSize = 11
		    g.Bold = True
		    Var theApogeeLabel As String = Format(pFlightData.pMaxAltitudeM, "0.0") + " m"
		    g.DrawText(theApogeeLabel, theApogeeX + 10, theApogeeY + 4)
		    g.Bold = False
		  End If

		  // X-axis label
		  g.DrawingColor = &c666666
		  g.FontSize = 11
		  g.DrawText("Time (seconds)", theChartLeft + (theChartWidth - g.TextWidth("Time (seconds)")) / 2, theHeight - 5)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function CalculateNiceStep(inMax As Double, inTargetSteps As Integer) As Double
		  // Calculate a nice step value for axis labels
		  Var theRough As Double = inMax / inTargetSteps
		  Var theMagnitude As Double = Pow(10, Floor(Log(theRough) / Log(10)))
		  Var theNormalized As Double = theRough / theMagnitude

		  Var theNiceStep As Double
		  If theNormalized <= 1 Then
		    theNiceStep = 1
		  ElseIf theNormalized <= 2 Then
		    theNiceStep = 2
		  ElseIf theNormalized <= 5 Then
		    theNiceStep = 5
		  Else
		    theNiceStep = 10
		  End If

		  Return theNiceStep * theMagnitude
		End Function
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pFlightData As FlightData
	#tag EndProperty


End Class
#tag EndClass
