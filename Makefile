include /opt/fpp/src/makefiles/common/setup.mk

all: libfpp-plugin-data2mqtt.so
debug: all

OBJECTS_fpp_data2mqtt_so += src/data2mqttPlugin.o src/MQTTItem.o
LIBS_fpp_data2mqtt_so += -L/opt/fpp/src -lfpp
CXXFLAGS_src/data2mqttPlugin.o += -I/opt/fpp/src

%.o: %.cpp Makefile
	$(CCACHE) $(CC) $(CFLAGS) $(CXXFLAGS) $(CXXFLAGS_$@) -c $< -o $@

libfpp-plugin-data2mqtt.so: $(OBJECTS_fpp_data2mqtt_so) /opt/fpp/src/libfpp.so
	$(CCACHE) $(CC) -shared $(CFLAGS_$@) $(OBJECTS_fpp_data2mqtt_so) $(LIBS_fpp_data2mqtt_so) $(LDFLAGS) -o $@

clean:
	rm -f libfpp-plugin-data2mqtt.so $(OBJECTS_fpp_data2mqtt_so)
