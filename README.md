<!---
  Copyright (c) 2021 for information on the respective copyright owner
  see the NOTICE file and/or the repository https://github.com/boschglobal/VDA-SiL-Standard

  SPDX-License-Identifier: Apache-2.0
-->

# Interface and Bus Schema for VDA SiLVI

## Introduction

This repository contains the interfaces and bus schema files of the VDA SiLVI standard.

Currently buffer representations of CAN, CAN-XL, Ethernet, Flexray and LIN are provided.

The schemas for CAN and Ethernet are an extension of the schemas at https://github.com/boschglobal/automotive-bus-schema for VDA SiLVI usage.

The Bus schemas in the Google FlatBuffers IDL describe automotive network communications within a virtualized simulation environment.

## Dependencies

Schemas in this repository require:

* FlatBuffers v1.12.0 or v2.0.0 (or later).

Later versions may require testing.

## Contribute

Please refer to [CONTRIBUTING.md](./CONTRIBUTING.md) for a quick read-up about what to consider if you want to contribute.

## License

Bus Schema for VDA SiLVI is open-sourced under the Apache-2.0 license. See the

[LICENSE](LICENSE) file for details.