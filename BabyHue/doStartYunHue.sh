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

#  Get the IP of the Hue
echo "Getting IP"

IP=""
while [ ${#IP} -le 7 ]
do
    IP=$(curl -L -k http://www.meethue.com/api/nupnp 2>/dev/null | egrep -o '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')
    if [ ${#IP} -le 7 ]; then
        sleep 10s
    fi
done

echo "Using IP: ${IP}"

#  Get and verify the user
#  TODO: check for username in Hue and user creation

echo "Getting and checking username"

USERNAME=$(cat $SCRIPT_LOCATION/hueUsername)

echo "Using username: ${USERNAME}"

API="http://${IP}/api/${USERNAME}/"

echo "${API}" > /tmp/hueApiUrl

echo "API location: ${API}"

#  Get and verify the BabyHue group

echo "Getting and checking Hue group"

GROUPS_URL="$(cat /tmp/hueApiUrl)groups/"
echo "Groups url: ${GROUPS_URL}"

GROUP_ID=$(curl -L -k ${GROUPS_URL} 2>/dev/null | egrep -o '[0-9]{1,2}\"\:\{\"name\"\:\"BabyHue' | egrep -o '[0-9]{1,2}')
if [ ${#GROUP_ID} -eq 0 ]; then
    echo "Creating Hue group"
    GROUP_ID=$(curl -H "Content-Type: application/json" -X POST -d '{"name":"BabyHue","lights":["1"]}' ${GROUPS_URL} 2>/dev/null | egrep -o 'success.*[0-9]{1,2}' | egrep -o '[0-9]{1,2}')
fi

echo "${GROUPS_URL}${GROUP_ID}/" > /tmp/hueGroupApi
echo "Hue group API: ${GROUPS_URL}${GROUP_ID}/"

echo "DONE"
