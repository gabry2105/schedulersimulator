
scheduler_simulator: scheduler_simulator.o clock.o io_device.o scheduler.o process_generator.o execute.o Round_Robin.o sem.o
	g++ scheduler_simulator.o clock.o io_device.o scheduler.o process_generator.o execute.o Round_Robin.o sem.o -o scheduler_simulator

scheduler_simulator.o: scheduler_simulator.cc mem.hh  queue.hh heap.hh std_queue.hh
	g++ -c scheduler_simulator.cc -o scheduler_simulator.o

clock.o: clock.cc task_struct.hh msg_struct.hh queue.hh mem.hh
	g++ -c clock.cc -o  clock.o

io_device.o: io_device.cc task_struct.hh msg_struct.hh queue.hh mem.hh 
	g++ -c io_device.cc -o io_device.o

scheduler.o: scheduler.cc task_struct.hh queue.hh mem.hh  msg_struct.hh
	g++ -c scheduler.cc -o scheduler.o

process_generator.o: process_generator.cc task_struct.hh queue.hh mem.hh
	g++ -c process_generator.cc -o process_generator.o

execute.o: execute.cc task_struct.hh mem.hh msg_struct.hh
	g++ -c execute.cc -o execute.o

Round_Robin.o: Round_Robin.cc task_struct.hh queue.hh mem.hh msg_struct.hh 
	g++ -c Round_Robin.cc -o Round_Robin.o

sem.o: sem.cc 
	g++ -c sem.cc -o sem.o

clean:
	rm *.o scheduler_simulator

.PHONY: clean
