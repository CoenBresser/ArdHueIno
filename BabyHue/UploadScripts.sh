#!/bin/sh

#  UploadScripts.sh
#  BabyHue
#
#  Created by Coen Bresser on 13/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

#scp root@10.0.0.1:/path/to/file.txt localfile.txt
sshpass -p arduino scp doStartYunHue.sh root@arduino.local:/usr/BabyHue/doStartYunHue.sh
sshpass -p arduino scp hueUsername root@arduino.local:/usr/BabyHue/hueUsername
sshpass -p arduino scp doNotifyGroup.sh root@arduino.local:/usr/BabyHue/doNotifyGroup.sh 