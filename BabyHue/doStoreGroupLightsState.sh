#!/bin/sh

#  doStoreGroupLightsState.sh restore light states from memory
#
#  BabyHue project to control the lights based on the noiselevel (of babies)
#
#  Created by Coen Bresser on 14/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

SCRIPT_LOCATION=$(dirname $0)

#  TODO: Check on existence of file

GROUP_URL="$(cat /tmp/hueGroupApi)"

LIGHTS=$(curl -L -k $(cat /tmp/hueGroupApi) 2>/dev/null| egrep -o 'lights.*"]' | egrep -o '("[0-9]{1,2}")' | egrep -o '[0-9]{1,2}')

for LIGHT in $LIGHTS
do
    LIGHT_URL="$(cat /tmp/hueApiUrl)lights/${LIGHT}"
    STATE=$(curl -L -k $LIGHT_URL 2>/dev/null | egrep -o '"(on|bri|hue|sat)":(true|false|[0-9]{1,5})')
    echo $STATE > "/tmp/Light${LIGHT}State"
done

echo "DONE"