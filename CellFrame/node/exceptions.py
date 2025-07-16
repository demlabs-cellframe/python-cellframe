class InsufficientFundsError(ValueError):
    def __init__(self, available_balance, requested_balance):
        super().__init__(
            f"Insufficient funds: available {available_balance}, requested {requested_balance}"
        )
        self.available_balance = available_balance
        self.requested_balance = requested_balance