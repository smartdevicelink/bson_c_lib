// swift-tools-version:5.3
import PackageDescription

let package = Package(
    name: "BiSON",
    defaultLocalization: "en",
    platforms: [
        .iOS(.v9)
    ],
    products: [
        .library(name: "BiSON")
    ],
    targets: [
        .target(
            name: "BiSON",
            path: "src",
            exclude: [
                "Makefile.am",
                "Makefile.in",
                "emhashmap/LICENSE",
                "emhashmap/README.mkd",
                "emhashmap/Makefile.in",
                "emhashmap/Makefile.am",
                "emhashmap/runtests.sh",
                "emhashmap/tests.c"
            ],
            publicHeadersPath: "."
        )
    ]
)