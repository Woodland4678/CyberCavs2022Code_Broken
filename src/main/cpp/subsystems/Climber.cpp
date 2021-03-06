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
#include <frc/Timer.h>
// ROBOTBUILDER TYPE: Subsystem.

// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=INCLUDES
#include "subsystems/Climber.h"
#include <frc/smartdashboard/SmartDashboard.h>


// END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=INCLUDES

//Climber positions
double straightUpPosition = -86.09;
double grabHighBarPosition = 56.5; //58.5
double swing1Position = 1.1588;
double liftOffMediumBarPosition = -9.61;
double swingToTraverseBarPosition = 243.5;
double grabTraverseBarPosition = 226.5;
double finalSwingPosition = 169.5;
double liftOffHighBarPosition = 139.5;

double positionIncrementForSlowMovement = 0.05; //When we want to move the arm against the bar we will keep increasing its target position by this much

double climberMotorAllowedError = 3;

auto delayForDetectingBar = 1_s; //how many counts we need to see the bar for before we move on in seconds
auto delayForPneumaticMovement = 1_s; //time delay to allow the pneumatics to move in seconds
auto delayForMotorMovement = 1_s;

auto originalTime = 0_s;
auto timeOut = 0_s;

Climber::Climber(){
    SetName("Climber");
    // BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DECLARATIONS
    SetSubsystem("Climber");

 AddChild("LevelLimit", &m_levelLimit);
 

 AddChild("LockLimit", &m_lockLimit);
 

 AddChild("ReadLimit2", &m_reachLimit2);
 

 AddChild("ReachLimit1", &m_reachLimit1);
 

 // AddChild("HeightSolenoid", &m_heightSolenoid);
 

 AddChild("LockSolenoid", &m_lockSolenoid);
 

 AddChild("ReachSolenoid", &m_reachSolenoid);
 
//rev::CANSparkMax m_followMotor{10, rev::CANSparkMax::MotorType::kBrushless};
 
 
//rev::CANSparkMax m_leaderMotor{9, rev::CANSparkMax::MotorType::kBrushless};
 
 

    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DECLARATIONS
    
    m_followMotor.Follow(m_leaderMotor);
    m_climberPidController.SetP(0.075);
    m_climberPidController.SetI(0);
    m_climberPidController.SetD(0);
    m_climberPidController.SetOutputRange(-0.85, 0.85);
}

void Climber::Periodic() {
    // Put code here to be run every loop
    frc::SmartDashboard::PutNumber("Climber Level", m_levelLimit.Get());
    frc::SmartDashboard::PutNumber("Climber Speed", m_climberEncoder.GetVelocity());
    frc::SmartDashboard::PutNumber("Climber Position", m_climberEncoder.GetPosition());
    frc::SmartDashboard::PutNumber("Climber Stage", climbStage);
}

void Climber::SimulationPeriodic() {
    // This method will be called once per scheduler run when in simulation

}
void Climber::SetClimberPower(double pwr) {
    //m_climberPidController.SetReference(pwr, rev::ControlType::kVelocity);
    m_leaderMotor.Set(pwr);
}
void Climber::IncreaseClimbState() {
    climbStage++;
}

bool isClicked = true;

bool Climber::CalibrateClimber(){
    if (!m_levelLimit.Get()&&isClicked){
        SetClimberPower(0.3);
    }

    else if(m_levelLimit.Get()){
        SetClimberPower(-0.3);
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
    m_reachSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
}

void Climber::ExtractReachingArm(){
     m_reachSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
}

void Climber::Lock(){
     m_lockSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
}

void Climber::Unlock(){
     m_lockSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
}

void Climber::LiftClimber(){
    // m_heightSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
}

void Climber::LowerClimber(){
    // m_heightSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
}

/*
enum {
    CLIMBINGSETUP,
    DRIVINGTOBAR,
    GRABHIGHBAR,
    ENSUREHIGHBAR,
    SWING1,
    LIFTOFFMIDBAR,
    MOVETOTRAVERSEBAR,
    GRABTRAVERSEBAR,
    ENSUREGRABTRAVERSEBAR,
    SWING2,
    LIFTOFFHIGHBAR
};
*/


void Climber::Climb(){
    
   switch (climbStage){
        case CLIMBINGSETUP:
            LiftClimber();
            SetClimberPosition(straightUpPosition);

            if (!(abs(m_climberEncoder.GetPosition() - straightUpPosition) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();
            }
            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                climbStage=DRIVINGTOBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }

            ExtendReachingArm();
            Unlock();

            break;

        case DRIVINGTOBAR:
            // drivebase: drive backward
            
            if (m_reachLimit1.Get() == true){ //if we do not see the bar keep resetting originalTime
                originalTime = frc::Timer::GetFPGATimestamp();
            }
            if ((frc::Timer::GetFPGATimestamp() - originalTime)>=delayForDetectingBar){
                climbStage=GRABHIGHBAR;
            }

            ///
            if (frc::Timer::GetFPGATimestamp() - timeOut > 5_s) {
                climbStage=GRABHIGHBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;

        case GRABHIGHBAR:
            SetClimberPosition(grabHighBarPosition);

            if (!(abs(m_climberEncoder.GetPosition() - grabHighBarPosition) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();
            }
            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                climbStage=ENSUREHIGHBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;
        
        case ENSUREHIGHBAR:
            grabHighBarPosition += positionIncrementForSlowMovement;
            SetClimberPosition(grabHighBarPosition);

            if (m_lockLimit.Get() == true){
                originalTime = frc::Timer::GetFPGATimestamp();
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime)>=delayForDetectingBar){      
                Lock();
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime)>=delayForDetectingBar+delayForPneumaticMovement){
                climbStage=SWING1;
                timeOut = frc::Timer::GetFPGATimestamp();
            }

            // Temporary code
            if (frc::Timer::GetFPGATimestamp() - timeOut > 3_s) {
                Lock();
            }
            
            //
            if (frc::Timer::GetFPGATimestamp() - timeOut > 4_s) {
                climbStage=SWING1;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;

        case SWING1: // swing
            SetClimberPosition(swing1Position); // need to find the position
            if (!(abs(m_climberEncoder.GetPosition() - swing1Position) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                climbStage=LIFTOFFMIDBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;
        
        case LIFTOFFMIDBAR: // lifting off the bar
            SetClimberPosition(liftOffMediumBarPosition); // need to find the position
            
            if (!(abs(m_climberEncoder.GetPosition() - liftOffMediumBarPosition) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();  
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                ExtractReachingArm(); 
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= (delayForDetectingBar+delayForPneumaticMovement)){
                climbStage=MOVETOTRAVERSEBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;
        
        case MOVETOTRAVERSEBAR:
            SetClimberPosition(swingToTraverseBarPosition); // find the position

            if (!(abs(m_climberEncoder.GetPosition() - swingToTraverseBarPosition) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();    
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                ExtendReachingArm();
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime)>=delayForDetectingBar+delayForPneumaticMovement){
                climbStage=GRABTRAVERSEBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;

        case GRABTRAVERSEBAR:
            SetClimberPosition(grabTraverseBarPosition);

            if (!(abs(m_climberEncoder.GetPosition() - grabTraverseBarPosition) < climberMotorAllowedError)){
                originalTime = frc::Timer::GetFPGATimestamp();      
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                climbStage=ENSUREGRABTRAVERSEBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;

        case ENSUREGRABTRAVERSEBAR:
            grabTraverseBarPosition -= positionIncrementForSlowMovement;
            SetClimberPosition(grabTraverseBarPosition);
            if (m_reachLimit2.Get() == true){
                originalTime = frc::Timer::GetFPGATimestamp();  
            }
            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                climbStage=SWING2;
                timeOut = frc::Timer::GetFPGATimestamp();
                Unlock();
            }


            // Temporary code
            if (frc::Timer::GetFPGATimestamp() - timeOut > 2_s) {
                Unlock();
            }
            
            if (frc::Timer::GetFPGATimestamp() - timeOut > 3_s) {
                climbStage=SWING2;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;

        case SWING2: // second swing
            SetClimberPosition(finalSwingPosition); // need to find the position
            
            if (!(abs(m_climberEncoder.GetPosition() - finalSwingPosition) < climberMotorAllowedError)){   
                originalTime = frc::Timer::GetFPGATimestamp();    
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime) >= delayForMotorMovement){
                Unlock();
            }

            if ((frc::Timer::GetFPGATimestamp() - originalTime)>=delayForDetectingBar+delayForPneumaticMovement){
                climbStage=LIFTOFFHIGHBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }

            // Temporary code
            if (frc::Timer::GetFPGATimestamp() - timeOut > 5_s) {
                climbStage=LIFTOFFHIGHBAR;
                timeOut = frc::Timer::GetFPGATimestamp();
            }
            break;

        case LIFTOFFHIGHBAR:
            SetClimberPosition(liftOffHighBarPosition); // find the position
            break;

        

    }
}

// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CMDPIDGETTERS

// END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=CMDPIDGETTERS


// Put methods for controlling this subsystem
// here. Call these from Commands.
