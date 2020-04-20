all:gkc-qml

MTime:=$(shell stat -c %Y qml.pro)
LastTime:=$(shell cat qml.pro.time2)

gkc-qml:
	@echo "Building gkc-qml ...depends_prefix: " ${depends_prefix} && \
	${depends_prefix}/native/bin/protoc --cpp_out=./ ./qt_native/paymentrequest.proto && \
	mkdir -p build  && \
	${depends_prefix}/qml/bin/lrelease qml.pro && \
	cd build && \
	if [ "$(LastTime)" = "" ]; then \
		echo $(MTime) > ../qml.pro.time; ${depends_prefix}/qml/bin/qmake ../qml.pro  "depends_prefix=$${depends_prefix}";  \
	else \
		if [ "$(MTime)" -gt "$(LastTime)" ]; then  echo $(MTime) > ../qml.pro.time; ${depends_prefix}/qml/bin/qmake ../qml.pro  "depends_prefix=$${depends_prefix}" ; fi; \
	fi && \
	$(MAKE) && \
	cd ../
clean:
	rm -rf build
	rm -rf qt_native/paymentrequest.pb.cc
	rm -rf qt_native/paymentrequest.pb.h
