#!/bin/sh

#  GetApiAddress.sh to get the api for the Hue
#  For now no check on the username is performed.
#  This should be done on the configuration website (or manually)
#
#  BabyHue project to control the lights based on the noiselevel (of babies)
#
#  Created by Coen Bresser on 13/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

#  TODO: change so that the current state is returned and in the arduino uses asynchronous calls

SCRIPT_LOCATION=$(dirname $0)

IP=""
while [ ${#IP} -le 7 ]
do
    IP=$(curl -L -k http://www.meethue.com/api/nupnp 2>/dev/null | egrep -o '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')
    if [ ${#IP} -le 7 ]; then
        sleep 10s
    fi
done

#  TODO: check for username and return that to the Arduino

USERNAME=$(cat $SCRIPT_LOCATION/hueUsername)

API="http://${IP}/api/${USERNAME}/"

echo "${API}" > /tmp/hueApiUrl
echo "OK"

