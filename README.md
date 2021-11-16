

## Building

### CMake configuration variables

  * `POLY_ETL_INCLUDE_DIR` - set to the ETL include directory to use. *Default external/etl*
  
### Platform configuration

`poly` has support for some platform specific functionality. Notably the
`power` module is platform dependant.

The default platform is the PC platform. This can be changed by defining
one of

  * `POLY_PLATFORM_NRF52840` - support nRF52840
  * `POLY_PLATFORM_TESTING` - testing support
  
This can be chosen via CMake by setting the `POLY_PLATFORM`
to either `nrf52840` or `testing` respective.

## License

This library is provided under [MIT license](LICENSE).

### Dependencies

  * `ETL` - [MIT license](LICENSE)
  * Parts of `libc++` - [Apache 2 with LLVM exception](LICENSE.APACHE2-LLVM)


