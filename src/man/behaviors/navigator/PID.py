import time

class PIDController:
    def __init__(self, proportional, integral, derivative):
        # PID control parameters
        self.proportionalGain = proportional
        self.integralGain = integral
        self.derivativeGain = derivative

        # State of PID controller
        self.lastTime = time.clock()
        self.integral = 0

    def correct(self, error):
        # Time
        newTime = time.clock()
        deltaTime = newTime - self.lastTime
        self.lastTime = newTime

        # Proportional term
        proportionalTerm = self.proportionalGain * error

        # Integral term
        self.integral += error * deltaTime
        integralTerm = self.integralGain * self.integral

        # Derivative term
        derivativeTerm = self.derivativeGain * (error / deltaTime)

        # Combine and return correction
        return proportionalTerm + integralTerm + derivativeTerm

    def reset(self):
        self.lastTime = time.clock()
        self.integral = 0
