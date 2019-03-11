#include <ros/ros.h>
#include <ros/package.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/String.h>

#include <string>
#include <iostream>


using namespace std;

geometry_msgs::Twist vel;
geometry_msgs::Twist vel_prev;
ros::Publisher cmd_vel_pub;
ros::Subscriber joy_sub;

void setSpeed(float linearSpeed, float angularSpeed)
{
	geometry_msgs::Twist vel_;
	vel_.linear.x = linearSpeed;
	vel_.angular.z = angularSpeed;
	//cmd_vel_pub.publish(vel_);
}

void joyLogitechTeleop(const sensor_msgs::Joy::ConstPtr& joy)
{
	
	//ROS_INFO("Joystick control: [%s]", to_string(joy->axes);

	if (joy->axes[1] >= 0.1 || joy->axes[1] <= -0.1)
	{
		vel.linear.x = 0.5*joy->axes[1];
	}
	if (joy->axes[1] >= 0.8 || joy->axes[1] <= -0.8)
	{
		vel.linear.x = joy->axes[1];
	}	
	if (joy->axes[0] >= 0.1 || joy->axes[0] <= -0.1)
	{
		vel.angular.z = 0.5*joy->axes[0];
	}	
	if (joy->axes[0] <= 0.1 && joy->axes[0] >= -0.1)
	{
		vel.angular.z = 0.0;
	}
	if (joy->axes[1] <= 0.1 && joy->axes[1] >= -0.1)
	{
		vel.linear.x = 0.0;
	}
	stringstream ss;
	ss << vel;
	string str = ss.str();
	cmd_vel_pub.publish(vel);
	cout << str;
}

void joyPS3Teleop(const sensor_msgs::Joy::ConstPtr& joy)
{
	vel_prev = vel;
	
	//ROS_INFO("Joystick control: [%s]", to_string(joy->axes);
    const int trn = 0; // turn left or right
    const int mv = 3; // move forward and backward
    const int stf = 2; //strafe left and right
    const int dm = 10; // deadman button

	// Just pressing deadman should stop the robot
	if (joy->buttons[dm] == 1)
	{
		vel.linear.x = 0.0;
		vel.linear.y = 0.0;
		vel.angular.z = 0.0;
	}
	// Forward and backward
	if ((joy->axes[mv] <= 0.1 && joy->axes[mv] >= -0.1) && (joy->buttons[dm] == 1))
	{
		vel.linear.x = 0.0;
	}
	if ((joy->axes[mv] >= 0.1 || joy->axes[mv] <= -0.1) && (joy->buttons[dm] == 1))
	{
		vel.linear.x = 0.5*joy->axes[mv];
	}
	if ((joy->axes[mv] >= 0.8 || joy->axes[mv] <= -0.8) && (joy->buttons[dm] == 1))
	{
		vel.linear.x = joy->axes[mv];
	}
	// Strafe
	if ((joy->axes[stf] <= 0.1 && joy->axes[stf] >= -0.1) && (joy->buttons[dm] == 1))
	{
		vel.linear.y = 0.0;
	}
	if ((joy->axes[stf] >= 0.1 || joy->axes[stf] <= -0.1) && (joy->buttons[dm] == 1))
	{
		vel.linear.y = 0.5*joy->axes[stf];
	}
	if ((joy->axes[stf] >= 0.8 || joy->axes[stf] <= -0.8) && (joy->buttons[dm] == 1))
	{
		vel.linear.y = joy->axes[stf];		
	}
	// Turn left and right
	if ((joy->axes[trn] <= 0.1 && joy->axes[trn] >= -0.1) && (joy->buttons[dm] == 1))
	{
		vel.angular.z = 0.0;
	}
	if ((joy->axes[trn] >= 0.1 || joy->axes[trn] <= -0.1) && (joy->buttons[dm] == 1))
	{
		vel.angular.z = 0.5*joy->axes[trn];
	}
    if ((vel_prev.linear.x != vel.linear.x) || (vel_prev.linear.y != vel.linear.y) || (vel_prev.angular.z != vel.angular.z))
    {
		stringstream ss;
		ss << vel;
		string str = ss.str();
		cout << str;
	}
	cmd_vel_pub.publish(vel);
}


int main(int argc, char** argv)
{
	// Initialise node
	ros::init(argc,argv,"pepper_joystick_teleop");
	// Create node handle
	ros::NodeHandle nh;
	// Publish velocity
	cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel",2);
	// Subscribe to joystick (rely on it being available)
	// Choose which one
	joy_sub = nh.subscribe<sensor_msgs::Joy>("joy",10,joyPS3Teleop);
	// Wait until Ctrl+C
	ros::spin();
	// Leave this wretched place
	return 0;
}



