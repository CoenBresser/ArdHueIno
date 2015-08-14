#!/bin/sh

#  doNotifyGroup.sh
#  BabyHue
#
#  Created by Coen Bresser on 14/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

SCRIPT_LOCATION=$(dirname $0)

#  TODO: Check on existence of file

GROUP_URL="$(cat /tmp/hueGroupApi)action/"

RESULT=$(curl -H "Content-Type: application/json" -X PUT -d '{"alert":"lselect"}' ${GROUP_URL} 2>/dev/null)
sleep 10s
RESULT=$(curl -H "Content-Type: application/json" -X PUT -d '{"alert":"none"}' ${GROUP_URL} 2>/dev/null)

#  Ignore the result
echo "DONE"