/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Button.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 10:41:47 by dbaladro          #+#    #+#             */
/*   Updated: 2026/03/03 10:44:50 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


class Button {
  private:
    int pin;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
    int buttonState;
    int lastButtonState;

  public:
    // Constructor: sets up the button when you create it
    Button(int pinNumber, unsigned long delayTime = 10);

    // Member functions
    void  begin();
    bool  isPressed();

};
