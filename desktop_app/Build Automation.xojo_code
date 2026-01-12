#tag BuildAutomation
			Begin BuildStepList Linux
				Begin BuildProjectStep Build
				End
			End
			Begin BuildStepList Mac OS X
				Begin BuildProjectStep Build
				End
				Begin CopyFilesBuildStep CopyHelp
					AppliesTo = 0
					Architecture = 0
					Target = 0
					Destination = 1
					Subdirectory = 
					FolderItem = Li4vSGVscC5odG1s
				End
				Begin IDEScriptBuildStep CodeSign , AppliesTo = 0, Architecture = 2, Target = 0
					Dim theAppPath As String = CurrentBuildLocation + "/" + CurrentBuildAppName + ".app"
				End
				Begin IDEScriptBuildStep Notarize , AppliesTo = 0, Architecture = 2, Target = 0
					Dim theAppPath As String = CurrentBuildLocation + "/" + CurrentBuildAppName + ".app"
					Dim theZipPath As String = CurrentBuildLocation + "/" + CurrentBuildAppName + ".zip"
				End
				Begin CopyFilesBuildStep CopySplashScreen
					AppliesTo = 0
					Architecture = 0
					Target = 0
					Destination = 1
					Subdirectory = 
					FolderItem = Li4vLi4vYXJ0d29yay9Nb2RlbFJvY2tldEF2aW9uaWM1MTJwdC5wbmc=
					FolderItem = Li4vLi4vYXJ0d29yay9Nb2RlbFJvY2tldEF2aW9uaWM1MTJwdEAyeC5wbmc=
				End
				Begin SignProjectStep Sign
				  DeveloperID=
				  macOSEntitlements={"App Sandbox":"False","Hardened Runtime":"False","Notarize":"False","UserEntitlements":""}
				End
			End
			Begin BuildStepList Windows
				Begin BuildProjectStep Build
				End
			End
#tag EndBuildAutomation
