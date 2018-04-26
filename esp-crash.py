#!/usr/bin/env python

import argparse
import re
import os
import subprocess
import sys


class ESPCrashParser(object):
    ESP_EXCEPTIONS = [
        "Illegal instruction",
        "SYSCALL instruction",
        "InstructionFetchError: Processor internal physical address or data error during instruction fetch",
        "LoadStoreError: Processor internal physical address or data error during load or store",
        "Level1Interrupt: Level-1 interrupt as indicated by set level-1 bits in the INTERRUPT register",
        "Alloca: MOVSP instruction, if caller's registers are not in the register file",
        "IntegerDivideByZero: QUOS, QUOU, REMS, or REMU divisor operand is zero",
        "reserved",
        "Privileged: Attempt to execute a privileged operation when CRING ? 0",
        "LoadStoreAlignmentCause: Load or store to an unaligned address",
        "reserved",
        "reserved",
        "InstrPIFDataError: PIF data error during instruction fetch",
        "LoadStorePIFDataError: Synchronous PIF data error during LoadStore access",
        "InstrPIFAddrError: PIF address error during instruction fetch",
        "LoadStorePIFAddrError: Synchronous PIF address error during LoadStore access",
        "InstTLBMiss: Error during Instruction TLB refill",
        "InstTLBMultiHit: Multiple instruction TLB entries matched",
        "InstFetchPrivilege: An instruction fetch referenced a virtual address at a ring level less than CRING",
        "reserved",
        "InstFetchProhibited: An instruction fetch referenced a page mapped with an attribute that does not permit instruction fetch",
        "reserved",
        "reserved",
        "reserved",
        "LoadStoreTLBMiss: Error during TLB refill for a load or store",
        "LoadStoreTLBMultiHit: Multiple TLB entries matched for a load or store",
        "LoadStorePrivilege: A load or store referenced a virtual address at a ring level less than CRING",
        "reserved",
        "LoadProhibited: A load referenced a page mapped with an attribute that does not permit loads",
        "StoreProhibited: A store referenced a page mapped with an attribute that does not permit stores"
    ]

    def __init__(self, toolchain_path, elf_path):
        self.toolchain_path = toolchain_path
        self.gdb_path = os.path.join(toolchain_path, "bin", "xtensa-lx106-elf-gdb")
        self.addr2line_path = os.path.join(toolchain_path, "bin", "xtensa-lx106-elf-addr2line")

        if not os.path.exists(self.gdb_path):
            raise Exception("GDB for ESP not found in {} - {} does not exist.\nUse --toolchain to point to "
                            "your toolchain folder.".format(self.toolchain_path, self.gdb_path))

        if not os.path.exists(self.addr2line_path):
            raise Exception("addr2line for ESP not found in {} - {} does not exist.\nUse --toolchain to point to "
                            "your toolchain folder.".format(self.toolchain_path, self.addr2line_path))

        self.elf_path = elf_path
        if not os.path.exists(self.elf_path):
            raise Exception("ELF file not found: '{}'".format(self.elf_path))

    def parse_text(self, text):
        print self.parse_exception(text)

        m = re.search('stack(.*)stack', text, flags = re.MULTILINE | re.DOTALL)
        if m:
            print "Stack trace:"
            for l in self.parse_stack(m.group(1)):
                print "  " + l
        else:
            print "No stack trace found."

    def parse_exception(self, text):
        m = re.search('Exception \(([0-9]*)\):', text)
        if m:
            exception_id = int(m.group(1))
            if 0 <= exception_id <= 29:
                return "Exception {}: {}".format(exception_id, ESPCrashParser.ESP_EXCEPTIONS[exception_id])
            else:
                return "Unknown exception: {}".format(exception_id)

    '''
    Decode one stack or backtrace.
    
    See: https://github.com/me-no-dev/EspExceptionDecoder/blob/master/src/EspExceptionDecoder.java#L402
    '''
    def parse_stack(self, text):
        r = re.compile('40[0-2][0-9a-fA-F]{5}\s')
        m = r.findall(text)
        return self.decode_function_addresses(m)

    def decode_function_address(self, address):
        args = [self.addr2line_path, "-e", self.elf_path, "-aipfC", address]
        return subprocess.check_output(args).strip()

    def decode_function_addresses(self, addresses):
        out = []
        for a in addresses:
            out.append(self.decode_function_address(a))
        return out

    '''
    GDB Should produce line number: https://github.com/me-no-dev/EspExceptionDecoder/commit/a78672da204151cc93979a96ed9f89139a73893f
    However it does not produce anything for me. So not using it for now.
    '''
    def decode_function_addresses_with_gdb(self, addresses):
        args = [self.gdb_path, "--batch"]

        # Disable user config file which might interfere here
        args.extend(["-iex", "set auto-load local-gdbinit off"])

        args.append(self.elf_path)

        args.extend(["-ex", "set listsize 1"])
        for address in addresses:
            args.append("-ex")
            args.append("l *0x{}".format(address))
        args.extend(["-ex", "q"])

        print "Running: {}".format(args)
        out = subprocess.check_output(args)
        print out

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--toolchain", help="Path to the Xtensa toolchain",
                        default=os.path.join(os.environ.get("HOME"), ".platformio/packages/toolchain-xtensa"))
    parser.add_argument("--elf", help="Path to the ELF file of the firmware",
                        default=".pioenvs/esp/firmware.elf")
    parser.add_argument("input", type=argparse.FileType('r'), default=sys.stdin)

    args = parser.parse_args()

    crash_parser = ESPCrashParser(args.toolchain, args.elf)
    crash_parser.parse_text(args.input.read())


if __name__ == '__main__':
    main()
