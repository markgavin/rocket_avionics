#tag BuildAutomation
			Begin BuildStepList Linux
			End
			Begin BuildStepList Mac OS X
				Begin SignProjectStep Sign
				End
			End
			Begin BuildStepList Windows
			End
			Begin BuildStepList iOS
				Begin BuildProjectStep Build
				End
				Begin CopyFilesBuildStep CopyFiles_Resources
					AppliesTo = 0
					Architecture = 0
					Target = 0
					Destination = 1
					Subdirectory = 
					FolderItem = Li4vLi4vYXJ0d29yay9Nb2RlbFJvY2tldEF2aW9uaWM1MTJwdEAyeC5wbmc=
					FolderItem = Li4vLi4vYXJ0d29yay9Nb2RlbFJvY2tldEF2aW9uaWM1MTJwdC5wbmc=
				End
				Begin SignProjectStep Sign
				End
			End
#tag EndBuildAutomation
