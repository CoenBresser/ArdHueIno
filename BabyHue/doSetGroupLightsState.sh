#!/bin/sh

#  doSetGroupLightsState.sh set the state of a group of lights
#
#  BabyHue project to control the lights based on the noiselevel (of babies)
#
#  Created by Coen Bresser on 14/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

SCRIPT_LOCATION=$(dirname $0)

GROUP_URL="$(cat /tmp/hueGroupApi)action/"
DATA='{"on":'${1}',"bri":'$2',"hue":'$3',"sat":'$4'}'
RESULT=$(curl -H "Content-Type: application/json" -X PUT -d $DATA ${GROUP_URL} 2>/dev/null)

echo "DONE"
