#!/bin/sh

#  UploadScripts.sh
#  BabyHue
#
#  Created by Coen Bresser on 13/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

#sftp root@10.0.0.1:/path/to/file.txt localfile.txt
sshpass -p arduino sftp root@arduino.local:/usr/BabyHue/GetApiAddress.sh GetApiAddress.sh
sshpass -p arduino sftp root@arduino.local:/usr/BabyHue/hueUsername hueUsername