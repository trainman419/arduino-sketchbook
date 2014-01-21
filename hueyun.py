#!/usr/bin/env python

from __future__ import print_function

import argparse
import json
import urllib
import sys

from time import sleep
import phue
from phue import Bridge


class HueBulb(object):
    def __init__(self, name, light):
        self._name = name
        self._light = light
        self._old_brightness = -1
        self._target_brightness = None

    def poll(self):
        """ Poll the HUE API for the current state of this bulb
        """
        if self._light.on:
            b = self._light.brightness
        else:
            b = 0
        if self._target_brightness is not None:
            if b == self._target_brightness:
                # reached target brightness
                self._target_brightness = None
            else:
                return
        if b != self._old_brightness:
            # brightness changed, send update
            print("%d"%(b))
            self._old_brightness = b

    def brightness(self, b):
        self._target_brightness = b
        if b == 0:
            #print("Turning %s off"%(self._name))
            self._light.on = False
        else:
            #print("Set brightness of %s to %d"%(self._name, b))
            self._light.on = True
            self._light.brightness = b

    def reset(self):
        #print("Resetting %s to default"%(self._name))
        self._light.on = True
        self._light.brightness = 255
        self._light.saturation = 0
        self._target_brightness = 255

    def is_normal(self):
        return self._light.on and self._light.brightness == 255 and \
                self._light.saturation == 0

    def is_on(self):
        return self._light.on

    def on(self):
        self._light.on = True

    def off(self):
        self._light.on = False

    def toggle(self):
        if self.is_on():
            if self.is_normal():
                self.off()
            else:
                self.reset()
        else:
            self.on()


def main():
    parser = argparse.ArgumentParser(description="Hue to Yun bridge")
    parser.add_argument("-b", "--bridge", help="Hue bridge")
    parser.add_argument("lights", nargs="+")

    args = parser.parse_args()

    if args.bridge is None:
        info = urllib.urlopen('http://www.meethue.com/api/nupnp').read()
        info = json.loads(info)
        if len(info) > 0:
            args.bridge = info[0][u'internalipaddress']
        else:
            print("ERROR: Could not auto-detect Hue bridge IP")
            print(" Please specify --bridge manually")
            sys.exit(1)

    bridge = None
    while bridge == None:
        try:
            bridge = Bridge(args.bridge)
        except phue.PhueRegistrationException as e:
            sleep(5)



    lights = []

    print("Lights:")
    for name, light in bridge.get_light_objects('name').iteritems():
        if name in args.lights:
            print(" - Found %s"%(name))
            lights.append(HueBulb(name, light))

    try:
        while True:
            i = raw_input()
            if i == "T":
                # TODO: toggle
                normal = True
                for light in lights:
                    if not light.is_normal():
                        normal = False
                for light in lights:
                    if normal:
                        light.off()
                    else:
                        light.reset()
            else:
                try:
                    v = int(i)
                    if v < 0:
                        v = 0
                    if v > 255:
                        v = 255
                    for light in lights:
                        light.brightness(v)
                except:
                    pass
            for light in lights:
                light.poll()
            # chill out man
            sleep(0.33)
    except KeyboardInterrupt:
        # try to exit quietly
        pass


if __name__ == '__main__':
    main()