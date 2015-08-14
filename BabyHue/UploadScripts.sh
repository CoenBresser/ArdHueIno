#!/bin/sh

#  UploadScripts.sh
#  BabyHue
#
#  Created by Coen Bresser on 13/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

#sshpass -p arduino scp doStartYunHue.sh root@arduino.local:/usr/BabyHue/doStartYunHue.sh
#sshpass -p arduino scp hueUsername root@arduino.local:/usr/BabyHue/hueUsername
#sshpass -p arduino scp doNotifyGroup.sh root@arduino.local:/usr/BabyHue/doNotifyGroup.sh
#sshpass -p arduino scp doStoreGroupLightsState.sh root@arduino.local:/usr/BabyHue/doStoreGroupLightsState.sh
#sshpass -p arduino scp doRestoreGroupLightsState.sh root@arduino.local:/usr/BabyHue/doRestoreGroupLightsState.sh
sshpass -p arduino scp doSetGroupLightsState.sh root@arduino.local:/usr/BabyHue/doSetGroupLightsState.sh

