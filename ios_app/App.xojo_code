#tag Class
Protected Class App
Inherits MobileApplication
	#tag Event
		Sub Opening()
		  // Initialize connection manager
		  pConnection = New FlightConnection

		  // Initialize current flight data
		  pCurrentFlight = New FlightData
		End Sub
	#tag EndEvent

	#tag Event
		Sub Closing()
		  // Clean up connection
		  If pConnection <> Nil Then
		    pConnection.Disconnect
		  End If
		End Sub
	#tag EndEvent


	#tag Property, Flags = &h0
		pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h0
		pCurrentFlight As FlightData
	#tag EndProperty


End Class
#tag EndClass
