#!/bin/sh

#  GetIpAddress.sh
#  BabyHue
#
#  Created by Coen Bresser on 13/08/15.
#  Copyright (c) 2015 Coen Bresser. All rights reserved.

OUTPUT="$(curl -L -k http://www.meethue.com/api/nupnp 2>/dev/null | egrep -o '[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}')"
echo "http://${OUTPUT}/api"

