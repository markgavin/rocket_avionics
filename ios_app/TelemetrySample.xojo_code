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

	#tag Property, Flags = &h0
		pRssi As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pSnr As Integer = 0
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


End Class
#tag EndClass
