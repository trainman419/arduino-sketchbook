#!/usr/bin/env python

from __future__ import print_function

import argparse
import json
import urllib
import sys
import select

from time import sleep
import phue
from phue import Bridge

out = open('/tmp/hueyun.log', 'w')
last = None

def log(s):
    out.write(s)
    out.flush()

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
            global last
            # brightness changed, send update
            if b != last:
                print("%d"%(b))
                sys.stdout.flush()
                log("Sending %s\n" %(repr(b)))
                last = b
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
            log("ERROR: Could not auto-detect Hue bridge IP\n")
            log(" Please specify --bridge manually\n")
            out.close()
            sys.exit(1)

    bridge = None
    while bridge == None:
        try:
            bridge = Bridge(args.bridge)
        except phue.PhueRegistrationException as e:
            sleep(5)


    lights = []

    log("Lights:\n")
    for name, light in bridge.get_light_objects('name').iteritems():
        if name in args.lights:
            log(" - Found %s\n"%(name))
            lights.append(HueBulb(name, light))

    try:
        while True:
            rlist, wlist, elist = select.select([sys.stdin], [], [], 0)
            i = None
            while rlist:
                i = raw_input()
                i = i.strip()
                log("Got %s\n" % (repr(i)) )
                rlist, wlist, elist = select.select([sys.stdin], [], [], 0)
            if i is not None:
                log("Processing %s\n" % (repr(i)) )
                if i == "T":
                    log("Toggle\n")
                    # toggle
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
                        log("%d\n"%(v))
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
    finally:
        out.close()


if __name__ == '__main__':
    main()
