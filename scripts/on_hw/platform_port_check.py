"""
Zephyr HW test
"""
import argparse
import json
import os
import sys
from nrf_dev_map import nrfmap


class test_board(object):
    def __init__(self, segger, platform):
        self._segger       = segger
        self._com_port     = ''
        self._platform_name = platform

    def get_port_com(self):
        self._com_port = nrfmap.ComPortMap.get_com_ports_by_id(self._segger, [nrfmap.Vendor.Segger])[0]

        return self._com_port

    def get_name(self):
        return self._platform_name


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument("-p", "--platform", help="Platform filter for testing")

    return parser.parse_args()


def conf_file_open(path):
    with open(os.path.expanduser(path)) as json_file:
        data = json.load(json_file)

    return data


def device_load(device_config):
    dev = []

    for p in device_config['devices']:
        dev.append(test_board(p['segger'], p['name']))

    return dev


def device_search_by_platform(boards, platform):
    for i in boards:
        if platform == i.get_name():
            return i

    return None


def main():
    option = parse_args()

    conf_patch = os.path.join(os.getcwd(), "scripts", "on_hw", "conf", "include_config.txt")

    f = open(conf_patch, "r")
    boards = f.readline()

    config = conf_file_open(boards)

    devices = device_load(config)

    device = device_search_by_platform(devices, option.platform)

    print(device.get_port_com())


if __name__ == "__main__":
    main()
