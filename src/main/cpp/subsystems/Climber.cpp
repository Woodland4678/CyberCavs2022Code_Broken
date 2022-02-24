// RobotBuilder Version: 4.0
//
// This file was generated by RobotBuilder. It contains sections of
// code that are automatically generated and assigned by robotbuilder.
// These sections will be updated in the future when you export to
// C++ from RobotBuilder. Do not put any code or make any change in
// the blocks indicating autogenerated code or it will be lost on an
// update. Deleting the comments indicating the section will prevent
// it from being updated in the future.

#include <stdio.h>
#include <Timer.h>
// ROBOTBUILDER TYPE: Subsystem.

// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=INCLUDES
#include "subsystems/Climber.h"
#include <frc/smartdashboard/SmartDashboard.h>


// END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=INCLUDES

//Climber positions
double straightUpPosition = 94.4;
double grabHighBarPosition = 239;
double swing1Position = 181.6588;
double liftOffMediumBarPosition = 170.89;
double swingToTraverseBarPosition = 424;
double grabTraverseBarPosition = 407;
double finalSwingPosition = 350;
double liftOffHighBarPosition = 320;

double positionIncrementForSlowMovement = 0.5; //When we want to move the arm against the bar we will keep increasing its target position by this much

double climberMotorAllowedError = 3;

double delayForDetectingBar = 1; //how many counts we need to see the bar for before we move on in seconds
double delayForPneumaticMovement = 1; //time delay to allow the pneumatics to move in seconds
double delayForMotorMovement = 1;

double originalTime = 0;
Climber::Climber(){
    SetName("Climber");
    // BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DECLARATIONS
    SetSubsystem("Climber");

 AddChild("LevelLimit", &m_levelLimit);
 

 AddChild("LockLimit", &m_lockLimit);
 

 AddChild("ReadLimit2", &m_reachLimit2);
 

 AddChild("ReachLimit1", &m_reachLimit1);
 

 AddChild("HeightSolenoid", &m_heightSolenoid);
 

 AddChild("LockSolenoid", &m_lockSolenoid);
 

 AddChild("ReachSolenoid", &m_reachSolenoid);
 
//rev::CANSparkMax m_followMotor{10, rev::CANSparkMax::MotorType::kBrushless};
 
 
//rev::CANSparkMax m_leaderMotor{9, rev::CANSparkMax::MotorType::kBrushless};
 
 

    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DECLARATIONS
    
    m_followMotor.Follow(m_leaderMotor);
    m_climberPidController.SetP(0.075);
    m_climberPidController.SetI(0);
    m_climberPidController.SetD(0);
    m_climberPidController.SetOutputRange(-0.50, 0.50);
}

void Climber::Periodic() {
    // Put code here to be run every loop
    frc::SmartDashboard::PutNumber("Climber Level", m_levelLimit.Get());
    frc::SmartDashboard::PutNumber("Climber Speed", m_climberEncoder.GetVelocity());
    frc::SmartDashboard::PutNumber("Climber Position", m_climberEncoder.GetPosition());

}

void Climber::SimulationPeriodic() {
    // This method will be called once per scheduler run when in simulation

}
void Climber::SetClimberPower(double pwr) {
    //m_climberPidController.SetReference(pwr, rev::ControlType::kVelocity);
    m_leaderMotor.Set(pwr);
}

bool isClicked = true;

bool Climber::CalibrateClimber(){
    if (!m_levelLimit.Get()&&isClicked){
        SetClimberPower(-0.3);
    }

    else if(m_levelLimit.Get()){
        SetClimberPower(0.3);
        isClicked=false;
    }else{
        m_climberEncoder.SetPosition(0);
        isClicked = true;
        m_leaderMotor.StopMotor();
        return true;
    }

    return false;
}


void Climber::SetClimberPosition(double position){
    m_climberPidController.SetReference(position, rev::ControlType::kPosition);
}

void Climber::ExtendReachingArm(){
    m_reachSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
}

void Climber::ExtractReachingArm(){
     m_reachSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
}

void Climber::Lock(){
     m_lockSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
}

void Climber::Unlock(){
     m_lockSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
}

void Climber::LiftClimber(){
    m_heightSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
}

void Climber::LowerClimber(){
    m_heightSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
}
void Climber::IncreaseClimbState() {
    climbStage++;
}



void Climber::Climb(){
   switch (climbStage){
        case 0:
            LiftClimber();
            SetClimberPosition(straightUpPosition);

            if (!(abs(m_climberEncoder.GetPosition() - straightUpPosition) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();
            }
            if ((frc::Timer::GetFPGATimestamp() - originalTime))>=delayForMotorMovement){
                climbStage=1;
                count=0;
            }

            ExtendReachingArm();
            Unlock();

            break;

        case 1:
            // drivebase: drive backward
            if (m_reachLimit1.Get() == true){ //if we do not see the bar keep resetting originalTime
                originalTime = frc::Timer::GetFPGATimestamp();
            }
            if ((frc::Timer::GetFPGATimestamp() - originalTime))>=delayForDetectingBar){
                climbStage=2;
                count=0;
            }
            break;

        case 2:
            SetClimberPosition(grabHighBarPosition);
            if (abs(m_climberEncoder.GetPosition() - grabHighBarPosition) < climberMotorAllowedError){
                count++;
            }

            if (count>=50){
                climbStage=3;
                count=0;
            }
            break;
        
        case 3:
            m_leaderMotor.Set(0.15);
            if (m_lockLimit.Get() == false){
                count++;
            }

            if (count>=50){      
                Lock();
            }

            if (count>=75){
                climbStage=4;
                count=0;
            }
            break;

        case 4: // swing
            SetClimberPosition(swing1Position); // need to find the position
            if (abs(m_climberEncoder.GetPosition() - swing1Position) < climberMotorAllowedError){
                count++;     
            }

            if (count>=50){
                climbStage=5;
            }
            break;
        
        case 5: // lifting off the bar
            SetClimberPosition(liftOffMediumBarPosition); // need to find the position
            
            
            if ((abs(m_climberEncoder.GetPosition() - liftOffMediumBarPosition) < climberMotorAllowedError)&&count<50){
                count++;  
            }

            if (count>=50){
                ExtractReachingArm();
                count++; 
            
            if (count>=75){
                climbStage=6;
                count=0;
            }
            break;
        
        case 6:
            SetClimberPosition(swingToTraverseBarPosition); // find the position
            if ((abs(m_climberEncoder.GetPosition() - swingToTraverseBarPosition) < climberMotorAllowedError)&&count<50){
                count++;     
            }

            if (count>=50){
                ExtendReachingArm();
                count++;
            }

            if (count>=75){
                climbStage=7;
                count=0;
            }
            break;

        case 7:
            SetClimberPosition(grabTraverseBarPosition);

            if ((abs(m_climberEncoder.GetPosition() - grabTraverseBarPosition) < climberMotorAllowedError)&&count<50){
                count++;     
            }

            if (count>=50){
                climbStage=8;
                count=0;
            }
            break;

        case 8:
            m_leaderMotor.Set(0.15);
            if (m_reachLimit2.Get() == false){
                count++;
            }
            if (count>=50){
                climbStage=9;
                count=0;
            }
            break;

        case 9: // second swing
            SetClimberPosition(finalSwingPosition); // need to find the position
            if ((abs(m_climberEncoder.GetPosition() - finalSwingPosition) < climberMotorAllowedError)&&count<50){
                count++;     
            }

            if (count>=50){
                Unlock();
                count++;
            }

            if (count>=75){
                climbStage=10;
                count=0;
            }
            break;


        case 10:
            SetClimberPosition(liftOffHighBarPosition); // find the position
            break;

        }

    }
}

// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CMDPIDGETTERS

// END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CMDPIDGETTERS


// Put methods for controlling this subsystem
// here. Call these from Commands.
