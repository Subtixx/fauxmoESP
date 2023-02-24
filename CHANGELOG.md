# fauxmoESP change log

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]

### Features

* add arduino json to LightStateChange ([b8ec6cb](https://github.com/Subtixx/fauxmoESP/commit/b8ec6cb572a8300fb2fb158421f3c8169e3b1f16))
* add external server request parsing ([877591f](https://github.com/Subtixx/fauxmoESP/commit/877591fb5c8228af820e76dcf78f7116c9854bf0))
* add initialize webserver to nullptr ([091a702](https://github.com/Subtixx/fauxmoESP/commit/091a7021e6aa55621af0b593fc6f16f03494b04d))
* add light state change class ([91e7d78](https://github.com/Subtixx/fauxmoESP/commit/91e7d78f2d227faa62b619b6ea385ab2ced6b98f))
* add light state changing ([61e58ed](https://github.com/Subtixx/fauxmoESP/commit/61e58edc75a7078517220c618713538e2c029c1e))
* add more api endpoints to openAPI ([197d484](https://github.com/Subtixx/fauxmoESP/commit/197d48444599922ad312ef47edf2e3d3c278bfc5))
* add openapi json file ([910f6ac](https://github.com/Subtixx/fauxmoESP/commit/910f6ac675a4c694968c4e040c078378376a1046))
* change README code ([31491e8](https://github.com/Subtixx/fauxmoESP/commit/31491e8a5046a9de8d5f9533f40cf75336e2b79c))
* modify README ([ee466b2](https://github.com/Subtixx/fauxmoESP/commit/ee466b2c331f21f9ceb554c0c0bb1a372d034a03))
* rewrite library to a full implementation ([98d4da2](https://github.com/Subtixx/fauxmoESP/commit/98d4da28f29fdfca9d0029baa242b8dc1640dd1d))


### Bug Fixes

* adding lights ([0a8ae6b](https://github.com/Subtixx/fauxmoESP/commit/0a8ae6b7e9bcac68e07422dd116d7cdac81e70dd))
* arduino json build outputs wrong json ([9dd00ca](https://github.com/Subtixx/fauxmoESP/commit/9dd00cae784365ad54c1c37178592b14b99c327d))
* wrong parsing light state change ardiunojson ([ecccb52](https://github.com/Subtixx/fauxmoESP/commit/ecccb528885bac369bc1ab5f8121f33833b09f96))


## [1.1.2] - 2023-02-22

- Fix minor issues with README
- Fix minor issues with docs

## [1.1.1] - 2023-02-22

- Fix issues with build system

## [1.1.0] - 2023-02-22

- Restructured the code to make it easier to maintain
- Change char pointers to String
- Added documentation to public methods

## [1.0.0] - 2023-02-21

- Initial release of the fork of fauxmoESP
- Added support for hue and saturation PR #2
- Added redirect option PR #3
- Fix: Device Unique ID length PR #1