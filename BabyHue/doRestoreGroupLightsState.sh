#!/bin/sh

#  doRestoreGroupLightsState.sh restore light states from memory
#
#  BabyHue project to control the lights based on the noiselevel (of babies)
#
#  Created by Coen Bresser on 13/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

SCRIPT_LOCATION=$(dirname $0)

#  TODO: Check on existence of files

#function join { local IFS="$1"; shift; echo "$*"; }

GROUP_URL="$(cat /tmp/hueGroupApi)"

LIGHTS=$(curl -L -k $(cat /tmp/hueGroupApi) 2>/dev/null| egrep -o 'lights.*"]' | egrep -o '("[0-9]{1,2}")' | egrep -o '[0-9]{1,2}')

SET_TO_OFF=0
for LIGHT in $LIGHTS
do
    STATE=$(cat "/tmp/Light${LIGHT}State")

    #  Pre process to set "on":true as the first and "on":false as the last element
    if [[ ${STATE:5:5} == "false" ]]; then
        STATE="${STATE:11} \"transitiontime\":0"
        SET_TO_OFF=1
    fi
    LIGHT_URL="$(cat /tmp/hueApiUrl)lights/${LIGHT}/state/"

    DATA=$(printf ",%s" $STATE)
    DATA="{${DATA:1}}"
    echo $DATA

    RESULT=$(curl -H "Content-Type: application/json" -X PUT -d $DATA $LIGHT_URL 2>/dev/null)

    if [ $SET_TO_OFF -gt 0 ]; then
        RESULT=$(curl -H "Content-Type: application/json" -X PUT -d '{"on":false}' $LIGHT_URL 2>/dev/null)
    fi

done

echo "DONE"