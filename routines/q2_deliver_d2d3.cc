
#include <stdexcept>

#include "dev/drive.h"
#include "util/tracker.h"
#include "common.h"

#include "q2_collect_d2d3.h"


void dropEggs(Robot& r, int n = 1) {
	// inch forward until the limit switch is hit
	r.drive.move({forward: 0.5, steer: 0});
	while(!(r.limits & (1 << 5)));
	r.drive.stop();

	for(int i = 0; i < n; i++) {
		if(i != 0) {
			delay(500); // to allow the previous egg to be removed
		}
		std::cout << "Dropping egg " << r.courier.egg(0) << std::endl;
		r.courier.unloadEgg();
	}

	// undo the straight motion (TODO: match time)
	r.drive.straight(-0.05, 0.5).wait();
}

void q2_deliver_d2d3(Robot& r)
{
	Tracker::instance()->logPose(0.2, -0.8, 0);
	r.drive.move({forward: 1, steer: 0.5});

	delay(1000);
	waitForLine(r, negate { LineSensors::Reading::NONE });
	std::cout << "Departed conveyor and found line" << std::endl;

	goToJunction(r, 0.30);
	turnAtJunction(r, 90);
	std::cout << "Looking up ramp" << std::endl;

	goToJunction(r, 999999); // ramps takes forever, so just drop the timeout
	turnAtJunction(r, 90);
	std::cout << "At plateau start" << std::endl;

	goToJunction(r, 0.86);
	std::cout << "At plateau center" << std::endl;
	goToJunction(r, 0.86);
	r.drive.straight(0.1).wait();
	std::cout << "At delivery nexus" << std::endl;

	while(r.courier.volume() != 0) {
		if(r.courier.egg(0) == EGG_WHITE) {
			// curve left, until we intersect the line
			r.drive.move({forward: 0.7, steer: 1});
			while(r.ls.read().lsc);
			while(!r.ls.read().lsc);
			r.drive.stop();

			// run until the limit switches go
			dropEggs(r, r.courier.egg(0) == EGG_WHITE ? 2 : 1);

			// undo the curved motion
			r.drive.move({forward: -0.7, steer: -1});
			while(r.ls.read().lsc);
			while(!r.ls.read().lsc);
			r.drive.stop();
		}
		else {
			dropEggs(r, 1);
		}
	}

	r.drive.straight(-0.1).wait();
	std::cout << "At delivery nexus" << std::endl;
}