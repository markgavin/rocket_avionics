#tag Class
Protected Class App
Inherits MobileApplication
	#tag Event
		Sub Opening()
		  // Initialize connection manager
		  pConnection = New FlightConnection

		  // Initialize current flight data
		  pCurrentFlight = New FlightData

		  // Load persisted rocket location
		  pRocketLocation = RocketLocation.Load()

		  // Initialize iPhone location services
		  Module_Location.Initialize()
		  Module_Location.StartUpdating()
		End Sub
	#tag EndEvent

	#tag Event
		Sub Closing()
		  // Save rocket location
		  If pRocketLocation <> Nil Then
		    pRocketLocation.Save()
		  End If

		  // Stop location updates
		  Module_Location.Shutdown()

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

	#tag Property, Flags = &h0
		pRocketLocation As RocketLocation
	#tag EndProperty


End Class
#tag EndClass
