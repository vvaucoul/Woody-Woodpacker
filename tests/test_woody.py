# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test_woody.py                                      :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/08 20:29:39 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/23 17:38:49 by mle-faou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import subprocess

# colors
GREEN = "\033[1;32m"
RED = "\033[1;31m"
YELLOW = "\033[1;33m"
BLINK = "\033[5m"
RESET = "\033[0;0m"

def individual_test(binary_name, isSilent=False, binary_arg=""):
    if not isSilent:
        print(YELLOW + "Testing " + binary_name + RESET)
        print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', binary_name], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Injector failed" + RESET
    if not isSilent:
        print(GREEN + "OK" + RESET)
        print("Checking packed binary's output...", end="")
    if binary_arg == "":
        original_output = b"....WOODY....\n" + subprocess.check_output([binary_name])
        woody_output = subprocess.check_output(['./woody'])
    else:
        original_output = b"....WOODY....\n" + subprocess.check_output([binary_name, binary_arg])
        woody_output = subprocess.check_output(['./woody', binary_arg])
    # print(f"\nog:[{original_output}]")
    # print(f"WP:[{woody_output}]")
    assert woody_output == original_output, RED + "Packed binary does not match original file" + RESET
    if not isSilent:
        print(GREEN + "OK" + RESET)

def test_objdump():
    # Execute the first command and redirect the output to .output
    with open('.output', 'w') as f:
        subprocess.run(['objdump', '-j', '.text', '-d', './woody'], stdout=f)

    # Execute the second command and redirect the output to .output2
    with open('.output2', 'w') as f:
        subprocess.run(['objdump', '-j', '.text', '-d', './tests/helloWorld'], stdout=f)

    # Execute the diff command to get the number of differences
    result = subprocess.run(['diff', '-y', '--suppress-common-lines', '.output', '.output2'], stdout=subprocess.PIPE)
    diff_lines = result.stdout.decode().split('\n')

    # Calculate the percentage of line differences
    total_lines = len(diff_lines) - 1
    num_diffs = total_lines - 1  # Exclude the header line from the count
    percent_diffs = (num_diffs / total_lines) * 100

    subprocess.run(['rm', '.output', '.output2'])

    return percent_diffs

def main():
    # Test with a valid file
    individual_test('./tests/helloWorld')

    # Test with no arguments
    print(YELLOW + "Testing nonexistent binary" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker'], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Failed with no arguments" + RESET
    print(GREEN + "OK" + RESET)

    # Test with help flag
    print(YELLOW + "Testing -h flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '-h'], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Failed with -h flag" + RESET
    print(GREEN + "OK" + RESET)

    print(YELLOW + "Testing --help flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '--help'], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Failed with --help flag" + RESET
    print(GREEN + "OK" + RESET)

    # Test with verbose flag
    print(YELLOW + "Testing verbose flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '-v'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Failed with -v flag" + RESET
    print(GREEN + "OK" + RESET)

    print(YELLOW + "Testing --verbose flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '--verbose'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Failed with --verbose flag" + RESET
    print(GREEN + "OK" + RESET)

    # Test with key flag
    print(YELLOW + "Testing key flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '-k=123'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Failed with -k flag" + RESET
    print(GREEN + "OK" + RESET)

    print(YELLOW + "Testing --key flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '--key=123'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Failed with --key flag" + RESET
    print(GREEN + "OK" + RESET)

    # Test with multiple flags
    print(YELLOW + "Testing multiple flags" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '-h', '-v', '-k=123'], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Failed with multiple flags" + RESET
    print(GREEN + "OK" + RESET)

    # Test with invalid key size
    print(YELLOW + "Testing invalid key size" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '-k=12345678901234567890'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Did not fail with invalid key size" + RESET
    print(GREEN + "OK" + RESET)

    # Test with multiple same flags
    print(YELLOW + "Testing multiple same flags" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '-h', '-h'], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Did not fail with multiple same flags" + RESET
    print(GREEN + "OK" + RESET)

    # Test with file flag
    print(YELLOW + "Testing file flag" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests/helloWorld'], stdout=subprocess.PIPE)
    assert result.returncode == 0, RED + "Failed with file flag" + RESET
    print(GREEN + "OK" + RESET)

    # Test with multiple file flags
    print(YELLOW + "Testing multiple file flags" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', '/tests/helloWorld', '/bin/ls'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Did not fail with multiple file flags" + RESET
    print(GREEN + "OK" + RESET)

    # Test with an invalid file
    print(YELLOW + "Testing nonexistent binary" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests/nonexistent'], stdout=subprocess.PIPE)
    assert result.returncode == 1, RED + "Did not fail with a nonexistent file" + RESET
    print(GREEN + "OK" + RESET)

    # Test with a directory
    print(YELLOW + "Testing dir" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests'], stdout=subprocess.PIPE)
    assert result.returncode == 1, RED + "Did not fail with a directory" + RESET
    print(GREEN + "OK" + RESET)

    # Test with a file with no read permissions
    print(YELLOW + "Testing no read permissions" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests/noRead'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Did not fail with a file with no read permissions" + RESET
    print(GREEN + "OK" + RESET)

    # Test with a file with no write permissions
    print(YELLOW + "Testing no write permissions" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests/noWrite'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Did not fail with a file with no write permissions" + RESET
    print(GREEN + "OK" + RESET)

    # Test with a file with no execute permissions
    print(YELLOW + "Testing no execute permissions" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests/noExecute'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Did not fail with a file with no execute permissions" + RESET
    print(GREEN + "OK" + RESET)

    # Test with a file with no permissions
    print(YELLOW + "Testing no permissions" + RESET)
    print("Checking injector...", end="")
    result = subprocess.run(['./woody_woodpacker', './tests/noPermissions'], stdout=subprocess.PIPE)
    assert result.returncode != 0, RED + "Did not fail with a file with no permissions" + RESET
    print(GREEN + "OK" + RESET)
    
    # Test with system binaries
    individual_test('/bin/ls')
    individual_test('/bin/echo', binary_arg="Hello World !")
    individual_test('/bin/cat', binary_arg="/etc/os-release")
    individual_test('/bin/hexdump', binary_arg="/bin/hexdump")
    individual_test('/bin/xxd', binary_arg="/bin/xxd")
    individual_test('/bin/nano', binary_arg="--version")
    individual_test('/bin/whoami')
    individual_test('/bin/md5sum', binary_arg="./woody")
    individual_test('/bin/mv', binary_arg="--version")
    individual_test('/bin/date')
    individual_test('/bin/pwd')
    individual_test('/bin/uname', binary_arg="-a")
    individual_test('/bin/df', binary_arg="-h")

    # Test text section binary diff from helloWorld
    diff_percent = test_objdump()
    print(YELLOW + "Testing text section binary diff" + RESET)
    print(GREEN + f"{diff_percent:.4f}% " + RESET + "different from original binary !")

    # End of tests
    print("\n" + GREEN + BLINK + "All tests passed !" + RESET)


if __name__ == "__main__":
    main()
