#tag Class
Protected Class TelemetrySample
	#tag Method, Flags = &h0
		Sub Constructor(inTimeMs As Integer, inAltitudeM As Double, inVelocityMps As Double)
		  // Initialize sample with specified values
		  pTimeMs = inTimeMs
		  pAltitudeM = inAltitudeM
		  pVelocityMps = inVelocityMps
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Constructor()
		  // Default constructor
		  pTimeMs = 0
		  pAltitudeM = 0.0
		  pVelocityMps = 0.0
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetTimeSeconds() As Double
		  // Return time as seconds (decimal)
		  Return pTimeMs / 1000.0
		End Function
	#tag EndMethod


	#tag Property, Flags = &h0
		pRocketId As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pTimeMs As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pAltitudeM As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pVelocityMps As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pPressurePa As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pTemperatureC As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pAccelX As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pAccelY As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pAccelZ As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pState As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pFlags As Integer = 0
	#tag EndProperty

	#tag ComputedProperty, Flags = &h0
		#tag Getter
			Get
			  // Check if orientation mode flag (0x80) is set
			  Return (pFlags And &h80) <> 0
			End Get
		#tag EndGetter
		IsOrientationMode As Boolean
	#tag EndComputedProperty

	#tag Property, Flags = &h0
		pRssi As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pSnr As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGroundPressurePa As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGroundAltitudeM As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pDifferentialAltitudeM As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsLatitude As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsLongitude As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsSpeedMps As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsHeadingDeg As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsSatellites As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsFix As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h0
		pMagX As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pMagY As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pMagZ As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGyroX As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGyroY As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGyroZ As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pPitch As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pRoll As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pHeading As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGatewayGpsFix As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h0
		pGatewayLatitude As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pGatewayLongitude As Double = 0.0
	#tag EndProperty

	#tag ComputedProperty, Flags = &h0
		#tag Getter
			Get
			  // Calculate distance to flight computer using Haversine formula
			  // Returns distance in meters
			  If Not pGpsFix Or Not pGatewayGpsFix Then
			    Return 0.0
			  End If

			  Const kEarthRadiusM As Double = 6371000.0
			  Const kDegToRad As Double = 0.01745329252

			  Var theLat1 As Double = pGatewayLatitude * kDegToRad
			  Var theLat2 As Double = pGpsLatitude * kDegToRad
			  Var theDeltaLat As Double = (pGpsLatitude - pGatewayLatitude) * kDegToRad
			  Var theDeltaLon As Double = (pGpsLongitude - pGatewayLongitude) * kDegToRad

			  Var theA As Double = Sin(theDeltaLat / 2) * Sin(theDeltaLat / 2) + _
			    Cos(theLat1) * Cos(theLat2) * Sin(theDeltaLon / 2) * Sin(theDeltaLon / 2)
			  Var theC As Double = 2 * ATan2(Sqrt(theA), Sqrt(1 - theA))

			  Return kEarthRadiusM * theC
			End Get
		#tag EndGetter
		DistanceToFlightM As Double
	#tag EndComputedProperty

	#tag ComputedProperty, Flags = &h0
		#tag Getter
			Get
			  // Calculate bearing from gateway to flight computer
			  // Returns bearing in degrees (0-360, 0=North)
			  If Not pGpsFix Or Not pGatewayGpsFix Then
			    Return 0.0
			  End If

			  Const kDegToRad As Double = 0.01745329252
			  Const kRadToDeg As Double = 57.2957795131

			  Var theLat1 As Double = pGatewayLatitude * kDegToRad
			  Var theLat2 As Double = pGpsLatitude * kDegToRad
			  Var theDeltaLon As Double = (pGpsLongitude - pGatewayLongitude) * kDegToRad

			  Var theX As Double = Sin(theDeltaLon) * Cos(theLat2)
			  Var theY As Double = Cos(theLat1) * Sin(theLat2) - Sin(theLat1) * Cos(theLat2) * Cos(theDeltaLon)

			  Var theBearing As Double = ATan2(theX, theY) * kRadToDeg

			  // Normalize to 0-360
			  If theBearing < 0 Then
			    theBearing = theBearing + 360.0
			  End If

			  Return theBearing
			End Get
		#tag EndGetter
		BearingToFlightDeg As Double
	#tag EndComputedProperty


End Class
#tag EndClass
