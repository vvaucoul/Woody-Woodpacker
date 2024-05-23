# Woody Woodpacker

## Description

- In this project the goal is to create a simple **packer** for **ELF64 binaries**. It will **add some instructions** to the **binary** and **encrypt the original code**. The binary will be able to **decrypt itself** at runtime to **execute the original code**.


## Usage

1. Clone the repository.
2. Run `make` to build the project.
3. Run `woody_woodpacker <binary>` to pack the binary.
3.1. You can use the `--help` option to see the usage.
4. Run `./woody` to run the packed binary.

## Basic test procedure

```bash
echo "int main(void) { printf(\"Hello\\\n\"); return 0;}" > hello.c; gcc -m64 -o hello hello.c
./woody_woodpacker hello
./woody

objdump -S woody > woody.obj; objdump -S hello > hello.obj
diff woody.obj hello.obj

echo "int main(void) { printf(\"Hello\\\n\"); return 0;}" > hello.c; gcc -m32 -o hello hello.c
./woody_woodpacker hello
./woody

rm hello.c hello hello.obj woody.obj
```

You can use `make test` to run a more in depth test.

## Encryption/Decryption in C

We're using a [Speck cipher](https://en.wikipedia.org/wiki/Speck_(cipher))  to encrypt the `.text` section of the binary. This cipher is a **lightweight block cipher** designed by the **NSA** and still **used to this day**.

The encryption is done in **assembly**, but here is the **C implementation** of the encryption/decryption functions.

*Macro defined in woody.h* :
```c
#define ROR(val, shift) ((val >> shift) | (val << (sizeof(val) * 8 - shift)))
#define ROL(x, n) (((x) << (n)) | ((x) >> (sizeof(x) * 8 - (n))))

#define R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)
#define INV_R(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))

#define ROUNDS 32
```

*Unused functions since the encryption is done in assembly :*

```c
void decrypt_text_section(size_t size, uint64_t *data, uint64_t key) {
    for (size_t i = 0; i < size; i += 2) {
        uint64_t y = data[i], x = data[i + 1];

        for (int j = 0; j < ROUNDS - 1; j++) {
            INV_R(x, y, key);
        }
        INV_R(x, y, key);

        data[i] = y;
        data[i + 1] = x;
    }
}

void encrypt_text_section2(size_t size, void *data, uint64_t key) {
    for (size_t i = 0; i < size; i += 2) {

        uint8_t x = ((uint8_t *)data)[i];
        uint8_t y = ((uint8_t *)data)[i + 1];

        R(x, y, key);
        for (int j = 0; j < ROUNDS - 1; j++) {
            R(x, y, key);
        }

        ((uint8_t *)data)[i] = x;
        ((uint8_t *)data)[i + 1] = y;
    }
}
```

## Useful commands

- `objdump -D <binary>` Disassemble the whole binary.

- `objdump -d <binary>`: Disassemble the binary and show the assembly code up to the `.fini` section.

- `readelf -S <binary>`: Display the sections' header of the binary and the offset of the `.text` section.

- `readelf -h <binary>`:  Display the ELF file header of the binary.
