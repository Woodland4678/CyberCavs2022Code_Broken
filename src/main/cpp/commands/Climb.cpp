// RobotBuilder Version: 4.0
//
// This file was generated by RobotBuilder. It contains sections of
// code that are automatically generated and assigned by robotbuilder.
// These sections will be updated in the future when you export to
// C++ from RobotBuilder. Do not put any code or make any change in
// the blocks indicating autogenerated code or it will be lost on an
// update. Deleting the comments indicating the section will prevent
// it from being updated in the future.

// ROBOTBUILDER TYPE: Command.

// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CONSTRUCTOR

#include "commands/Climb.h"
#include <frc/smartdashboard/SmartDashboard.h>

Climb::Climb(Climber* m_climber)
:m_climber(m_climber){

    // Use AddRequirements() here to declare subsystem dependencies
    // eg. AddRequirements(m_Subsystem);
    SetName("Climb");
    AddRequirements({m_climber});

// END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CONSTRUCTOR

}

bool done = false;
// Called just before this Command runs the first time
void Climb::Initialize() {

}

int count = 0;
bool doneCalibrate = false;
// Called repeatedly when this Command is scheduled to run
void Climb::Execute() {
    // m_climber->SetClimberPower(0.15);
    if (!doneCalibrate){
        doneCalibrate = m_climber->CalibrateClimber();
    }
    
    else if (doneCalibrate){
        count++;
    }
    frc::SmartDashboard::PutNumber("Climb command count", count);
    if (count>=50){
        m_climber->SetClimberPosition(94.4);
        if (count >= 75) {
            done = true;
            count = 0;
            doneCalibrate = false;
        }
    }else{
        done = false;
    }

}

// Make this return true when this Command no longer needs to run execute()
bool Climb::IsFinished() {
    return done;
}

// Called once after isFinished returns true
void Climb::End(bool interrupted) {

}

bool Climb::RunsWhenDisabled() const {
    // BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DISABLED
    return false;

    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DISABLED
}
