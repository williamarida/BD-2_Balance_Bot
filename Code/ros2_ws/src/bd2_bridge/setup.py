from setuptools import setup

setup(
    name="bd2_bridge", version="0.1.0", packages=["bd2_bridge"],
    data_files=[
        ("share/ament_index/resource_index/packages", ["resource/bd2_bridge"]),
        ("share/bd2_bridge", ["package.xml"]),
    ],
    install_requires=["setuptools"], zip_safe=True,
    maintainer="BD-2 project", maintainer_email="maintainer@example.com",
    description="BD-2 bench telemetry and settings bridge",
    license="LicenseRef-Proprietary",
    entry_points={"console_scripts": ["bridge = bd2_bridge.node:main"]},
)
