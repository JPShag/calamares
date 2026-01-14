# netprivacy - Calamares Network Privacy Module

Pre-network MAC spoofing and IPv6 privacy configuration for Calamares installers.

## Features

- **MAC Spoofing**: Disabled, Random, Vendor-targeted, or Fixed
- **IPv6 Privacy**: Standard, Privacy Extensions (RFC 4941), or Disabled
- 20 built-in vendor OUIs with support for custom vendors
- Works with NetworkManager and systemd-networkd

## Files Generated

When enabled, the module writes configuration to the target system:

**MAC Spoofing:**
- `/usr/lib/NetworkManager/conf.d/80-calamares-mac-privacy.conf`
- `/usr/lib/systemd/network/80-calamares-mac-privacy.link`
- `/etc/NetworkManager/dispatcher.d/80-vendor-mac.sh` (vendor mode only)

**IPv6 Privacy Extensions:**
- `/usr/lib/NetworkManager/conf.d/80-calamares-ipv6-privacy.conf`
- `/usr/lib/systemd/networkd.conf.d/80_ipv6-privacy-extensions.conf`

**IPv6 Disabled:**
- `/etc/sysctl.d/99-calamares-disable-ipv6.conf`

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

## Integration

Add to your `settings.conf`:

```yaml
sequence:
  - show:
      - welcome
      - locale
      - netprivacy   # <-- add here
      - keyboard
      - partition
      - users
      - summary
  - exec:
      - netprivacy   # <-- and here
      - ...
```

## Configuration

See `netprivacy.conf` for available options.

## License

GPL-3.0-or-later

## Author

JPShag (2025)
