#ifndef PROVISION_HTML_H
#define PROVISION_HTML_H

#include <Arduino.h>

// HTML content is broken into parts to avoid compiler limits and allow injection.

// Part 1: Head and start of body/styles
static constexpr const char index_html1[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <title>)rawliteral";
// Config: HTML_TITLE injected here
static constexpr const char index_html2[] PROGMEM = R"rawliteral(</title>
    <meta charset="UTF-8" />
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1.0, maximum-scale=1"
    />
    <style>
      :root {
        --card-background: #ffffff;
        --background-color: #f9f9f9;
        --font-color: #1a1f36;
        --theme-color: )rawliteral";
// Config: THEME_COLOR injected here
static constexpr const char index_html3[] PROGMEM = R"rawliteral(;
        --shadow-color: rgba(0, 0, 0, 0.1);
        --border-color: #cccccc;
        /* Define theme color RGB parts for focus ring */
         --theme-color-r: 30; /* Default: dodgerblue */
         --theme-color-g: 144;
         --theme-color-b: 255;
      }

      /* ... (Rest of CSS is unchanged from previous correct version) ... */
       * {
        font-family: Poppins, sans-serif;
        color: var(--font-color);
      }

      body {
        touch-action: manipulation;
        color: var(--font-color);
        background-color: var(--background-color);
        margin: 2rem 0;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        min-height: 100vh;
        box-sizing: border-box; /* Prevent padding/margins from causing overflow */
      }

      table {
        border-collapse: collapse;
        width: 100%;
        margin-bottom: 0.5rem;
        margin-top: 0.5rem;
      }

      th,
      td {
        padding: 0.75rem;
        text-align: left;
        vertical-align: middle; /* Align radio buttons nicely */
      }

      th {
        font-weight: bold;
        color: #333333;
      }

      .header {
        text-align: center;
        width: 100%; /* Ensure header takes full width */
        padding: 0 1rem; /* Add some padding */
        box-sizing: border-box;
      }

      /* Radio Button Styles */
      @supports (-webkit-appearance: none) or (-moz-appearance: none) {
        input[type="radio"] {
          --active: var(--theme-color);
          --active-inner: var(--card-background);
          --border: var(--border-color); /* Use variable */
          --border-hover: var(--theme-color);
          --background: var(--card-background);
          -webkit-appearance: none;
          -moz-appearance: none;
          height: 1.3125rem;
          outline: none;
          display: inline-block;
          vertical-align: middle;
          position: relative;
          margin: 0;
          cursor: pointer;
          border: 1px solid var(--bc, var(--border)); /* Use variable */
          background: var(--b, var(--background));
          transition: background 0.3s, border-color 0.3s, box-shadow 0.2s;
          -webkit-tap-highlight-color: transparent; /* Remove tap highlight */
        }
        input[type="radio"]:after {
            content: '';
            display: block;
            left: 0;
            top: 0;
            position: absolute;
            transition: transform .3s ease, opacity .2s ease;
        }
        input[type="radio"]:checked {
            --b: var(--active);
            --bc: var(--active);
            --d-o: .3s;
            --d-t: .6s;
            --d-t-e: cubic-bezier(.2, .85, .32, 1.2);
        }
        input[type="radio"]:disabled {
            --b: var(--disabled-inner); /* Adjust disabled appearance if needed */
            cursor: not-allowed;
            opacity: .7; /* Slightly faded */
         }
        input[type="radio"]:disabled:checked {
            --b: var(--disabled);
            --bc: var(--border); /* Keep border */
         }
        input[type="radio"]:disabled + label {
            cursor: not-allowed;
        }
        input[type="radio"]:hover:not(:checked):not(:disabled) {
           --bc: var(--border-hover);
        }
        input[type="radio"]:focus {
          box-shadow: 0 0 0 2px rgba(var(--theme-color-r), var(--theme-color-g), var(--theme-color-b), 0.2);
         }
        input[type="radio"]:not(.switch) {
          width: 1.3125rem; /* Match height */
        }
        input[type="radio"]:not(.switch):after {
            opacity: var(--o, 0);
         }
         input[type="radio"]:not(.switch):checked {
            --o: 1;
         }

        input[type="radio"] + label {
          font-size: 1em; /* Adjust label size */
          line-height: 1.3125rem; /* Match radio height */
          display: inline-block; /* Keep label inline */
          vertical-align: middle;
          cursor: pointer;
          margin-left: .5rem; /* Space between radio and label */
        }


         /* Specific styles for circle radio */
        input[type="radio"].circle { /* Added class for clarity */
            border-radius: 50%;
        }
        input[type="radio"].circle:after {
            width: 1.3125rem; /* Match radio size */
            height: 1.3125rem;
            border-radius: 50%;
            background: var(--active-inner);
            opacity: 0;
            transform: scale(0.7); /* Inner circle smaller */
            box-sizing: border-box; /* Include border in size */
            border: 2px solid var(--active-inner); /* Match background */
        }
         input[type="radio"].circle:checked:after {
            background: var(--active); /* Fill with active color */
             border-color: var(--active-inner); /* Border contrasts */
            opacity: 1;
            transform: scale(0.5); /* Adjust final size */
        }


      }

      @supports (not (-webkit-appearance: none)) and
        (not (-moz-appearance: none)) {
        input[type="radio"]:checked {
          accent-color: var(--theme-color);
        }
      }


      a:disabled,
      input:disabled,
      button:disabled {
        cursor: not-allowed;
        pointer-events: none;
        opacity: 0.5;
      }

      .signal,
      .radiossid {
        text-align: center;
      }

      .card {
        background-color: var(--card-background);
        box-shadow: 0 0.25rem 0.75rem var(--shadow-color);
        border-radius: 0.75rem;
        max-width: 50rem;
        width: calc(100% - 2rem); /* Reduced margin for smaller screens */
        padding: 1.5rem; /* Slightly reduced padding */
        margin: 1rem auto; /* Center card horizontally */
        text-align: center;
        box-sizing: border-box;
      }

      .container {
        padding-top: 1.25rem;
        text-align: center;
      }

      #hidden-network-selector {
        /* Adjusted margins for better spacing */
        margin: 1rem 10% 0;
        text-align: left;
      }
       #hidden-network-selector p { /* Style the label text */
          margin-bottom: 0.5rem;
          font-weight: bold;
      }
      #hidden-network-selector div { /* Style the radio container */
          display: flex;
          align-items: center;
      }
       #hidden-network-selector input[type="radio"] { /* Add margin to radio */
          margin-right: 0.5rem;
      }

      .icon_button {
        -webkit-tap-highlight-color: transparent;
        background-color: transparent;
        background-repeat: no-repeat;
        border: none;
        cursor: pointer;
        overflow: hidden;
        outline: none;
        padding: 0; /* Remove default padding */
        vertical-align: middle; /* Align icon better */
      }

      .icn-spinner {
        animation: spin-animation 0.5s infinite linear; /* Added linear */
        display: inline-block;
      }

      @keyframes spin-animation {
        0% {
          transform: rotate(0deg);
        }

        100% {
          transform: rotate(359deg);
        }
      }

      .textinput {
        width: 100%; /* Make inputs take full width of container */
        box-sizing: border-box;
        /* max-width: 18.75rem; remove max width */
        padding: 0.75rem; /* Increased padding */
        border: 1px solid var(--border-color); /* Thinner border */
        -webkit-border-radius: 0.3125rem;
        border-radius: 0.3125rem;
        -webkit-transition: 0.5s;
        transition: 0.5s;
        outline: none;
        margin-top: 0.25rem; /* Add space below label */
      }
      
      /* Specific padding for password to accommodate eye icon */
      .password-container input[type="password"],
      .password-container input[type="text"] {
           padding-right: 2.5rem; /* Make space for the icon */
      }


      /* Container for password input and eye icon */
      .password-container {
        position: relative;
        width: 75%; /* Match other inputs */
        max-width: 18.75rem; /* Re-apply max-width here */
        margin: 0 auto; /* Center the container */
      }

      .password-container .icon_button {
          position: absolute;
          right: 0.5rem; /* Position icon inside */
          top: 50%;
          transform: translateY(-50%); /* Center vertically */
          height: 100%; /* Ensure button is clickable */
          display: flex; /* Use flex to center svg */
          align-items: center;
      }


      .textinput:focus {
        border-color: var(--theme-color);
        box-shadow: 0 0 0 2px rgba(var(--theme-color-r), var(--theme-color-g), var(--theme-color-b), 0.2);
      }

      label {
        display: block;
        font-size: 0.9rem;
        font-weight: bold; /* Make labels bolder */
        padding-bottom: 0.125rem;
        text-align: left; /* Align labels left */
        width: 75%; /* Match input width */
        max-width: 18.75rem; /* Match input width */
        margin: 0 auto; /* Center label */
      }
      /* Specific label override for radio buttons */
      td label, #hidden-network-selector label {
         width: auto;
         margin: 0;
         font-weight: normal;
         cursor: pointer;
         display: inline; /* Keep label inline with radio */
         text-align: left;
      }


      #footer .copyright {
        width: 100%;
        margin: 2.5em 0 2em 0;
        font-size: 0.8em;
        text-align: center;
      }

      .error-message {
        color: #cc0033;
        display: block;
        font-size: 0.75rem;
        line-height: 0.9375rem;
        margin: 0.3125rem auto 0; /* Center error message */
        text-align: left; /* Align text left */
        width: 75%; /* Match input width */
        max-width: 18.75rem; /* Match input width */
        min-height: 0.9375rem; /* Reserve space */
      }

      .error input[type="text"],
      .error input[type="password"] {
        background-color: #fce4e4;
        border-color: #cc0033;
      }

      .btn-process {
        -webkit-tap-highlight-color: transparent;
        background-color: var(--theme-color);
        box-shadow: 0 0.25rem 0.5rem var(--shadow-color),
          0 0.5rem 1.5rem var(--shadow-color);
        font-size: 1rem;
        font-weight: bold; /* Bolder text */
        border: none;
        outline: none;
        padding: 0.75rem 1.5rem; /* More padding */
        border-radius: 0.3125rem;
        color: #fff;
        cursor: pointer;
        transition: background-color 0.2s ease, transform 0.1s ease; /* Add transitions */
      }
      .btn-process:hover:not(:disabled) {
           background-color: color-mix(in srgb, var(--theme-color) 85%, black); /* Darken on hover */
      }
       .btn-process:active:not(:disabled) {
           transform: translateY(1px); /* Press down effect */
           box-shadow: 0 0.1rem 0.3rem var(--shadow-color);
      }


      .connect-header {
        margin-top: 0rem;
        margin-bottom: 1rem; /* Add space below header */
        padding: 0;
        text-align: center;
      }

      /* Spinner animation (unchanged) */
      #connecting-ring:after {
        content: "";
        display: inline-block;
        width: 1em;
        margin-left: 0.5rem;
        height: 1em;
        vertical-align: middle;
        border-radius: 50%;
        border: 0.1875rem solid #fff;
        border-color: #fff transparent #fff transparent;
        animation: ring 1.2s linear infinite;
      }

      @keyframes ring {
        0% {
          transform: rotate(0deg);
        }

        100% {
          transform: rotate(360deg);
        }
      }
      
       /* Success Checkmark animation (unchanged) */
      .wrapper {
        display: flex;
        justify-content: center;
        align-items: center;
      }

      .checkmark__circle {
        stroke-dasharray: 166;
        stroke-dashoffset: 166;
        stroke-width: 2;
        stroke-miterlimit: 10;
        stroke: #7ac142;
        fill: none;
        animation: stroke 0.6s cubic-bezier(0.65, 0, 0.45, 1) forwards;
      }

      .checkmark {
        width: 3.5rem;
        height: 3.5rem;
        border-radius: 50%;
        display: block;
        stroke-width: 2;
        stroke: #fff;
        stroke-miterlimit: 10;
        margin-top: 3rem;
        box-shadow: inset 0rem 0rem 0rem #7ac142;
        animation: fill 0.4s ease-in-out 0.4s forwards,
          scale 0.3s ease-in-out 0.9s both;
      }

      .checkmark__check {
        transform-origin: 50% 50%;
        stroke-dasharray: 48;
        stroke-dashoffset: 48;
        animation: stroke 0.3s cubic-bezier(0.65, 0, 0.45, 1) 0.8s forwards;
      }

      @keyframes stroke {
        100% {
          stroke-dashoffset: 0;
        }
      }

      @keyframes scale {
        0%,
        100% {
          transform: none;
        }

        50% {
          transform: scale3d(1.1, 1.1, 1);
        }
      }

      @keyframes fill {
        100% {
          box-shadow: inset 0rem 0rem 0rem 1.875rem #7ac142;
        }
      }
      
      /* Media Query for smaller screens */
       @media (max-width: 600px) {
           body {
               margin: 1rem 0; /* Reduce body margin */
           }
           .card {
               width: calc(100% - 1rem); /* Make card almost full width */
               padding: 1rem; /* Reduce card padding */
               margin: 0.5rem auto;
           }
            .textinput, label, .error-message, .password-container {
               width: 90%; /* Increase width for inputs/labels */
               max-width: none; /* Remove max-width */
           }
            #hidden-network-selector {
               margin: 1rem 5% 0; /* Adjust margins */
           }
            th, td {
                padding: 0.5rem; /* Reduce table padding */
            }
       }
    </style>
  </head>

  <body>
    <div class="header">
      <div id="logo-container">)rawliteral";
// Config: SVG_LOGO injected here
static constexpr const char index_html4[] PROGMEM = R"rawliteral(</div>
      <h2 id="app-title" style="margin-top: 0.5rem; margin-bottom: 0rem"></h2>
      <h4 id="app-subtitle" style="margin-top: 0.2rem"></h4>
      <p
        id="info-text"
        style="
          opacity: 0.5;
          word-wrap: break-word;
          overflow-wrap: break-word;
          max-width: 90%;
          margin: 0 auto;
          text-align: center;
        "
      ></p>
    </div>
    <div id="main-card" class="card">
      <form id="network_form" name="network_form">
        <h3 class="connect-header">Connect to Wifi</h3>
        <div style="overflow-x: auto">
          <table>
            <thead>
              <tr>
                <th class="radiossid">
                  <button
                    class="icon_button"
                    type="button"
                    title="Refresh Network List"
                    onclick="loadSSID();"
                  >
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      id="refresh-icon"
                      style="vertical-align: -0.125em"
                      width="1em"
                      height="1em"
                      preserveaspectratio="xMidYMid meet"
                      viewbox="0 0 1536 1536"
                    >
                      <path
                        fill="var(--font-color)"
                        d="M1511 928q0 5-1 7q-64 268-268 434.5T764 1536q-146 0-282.5-55T238 1324l-129 129q-19 19-45 19t-45-19t-19-45V960q0-26 19-45t45-19h448q26 0 45 19t19 45t-19 45l-137 137q71 66 161 102t187 36q134 0 250-65t186-179q11-17 53-117q8-23 30-23h192q13 0 22.5 9.5t9.5 22.5zm25-800v448q0 26-19 45t-45 19h-448q-26 0-45-19t-19-45t19-45l138-138Q969 256 768 256q-134 0-250 65T332 500q-11 17-53 117q-8 23-30 23H50q-13 0-22.5-9.5T18 608v-7q65-268 270-434.5T768 0q146 0 284 55.5T1297 212l130-129q19-19 45-19t45 19t19 45z"
                      ></path>
                    </svg>
                  </button>
                </th>
                <th>SSID</th>
                <th class="signal">Signal</th>
              </tr>
            </thead>
            <tbody id="table-body">
              <!-- Network rows will be inserted here by JS -->
              <tr><td colspan="3" style="text-align: center; color: grey;">Initializing...</td></tr> <!-- Initial placeholder -->
            </tbody>
          </table>
        </div>
        <div id="hidden-network-selector">
          <p>Hidden Network?</p>
          <div>
            <input
              type="radio"
              id="hiddennetwork_radio"
              name="ssid"
              value="hiddennetwork"
              class="circle"
              onclick="onRadio(this);"
            />
             <label for="hiddennetwork_radio">Specify Hidden Network</label>
          </div>
        </div>
        <div class="container" id="hiddenNetwork" style="display: none">
          <label for="ssid">Network SSID</label>
          <div id="error-ssid-input">
            <input
              type="text"
              name="hidden_ssid"
              id="ssid"
              class="textinput"
              placeholder="SSID"
              aria-label="Hidden Network SSID"
            />
            <div id="error-ssid-message" class="error-message"></div>
          </div>
        </div>
        <div class="container" id="hiddenPassword" style="display: none">
          <label for="password">Network Password</label>
          <div id="error-password-input" class="password-container">
            <input
              type="password"
              name="password"
              id="password"
              class="textinput"
              placeholder="Password"
              aria-label="Network Password"
            />
            <button
                class="icon_button"
                type="button"
                title="Show/Hide Password"
                onclick="togglePassShow('password', 'eye_icon');"
              >
                <svg
                  xmlns="http://www.w3.org/2000/svg"
                  style="vertical-align: middle;" width="1em" height="1em"
                  preserveaspectratio="xMidYMid meet"
                  viewbox="0 0 24 24"
                >
                  <path
                    id="eye_icon"
                    fill="var(--font-color)"
                    d="M10.94 6.08A6.93 6.93 0 0 1 12 6c3.18 0 6.17 2.29 7.91 6a15.23 15.23 0 0 1-.9 1.64a1 1 0 0 0-.16.55a1 1 0 0 0 1.86.5a15.77 15.77 0 0 0 1.21-2.3a1 1 0 0 0 0-.79C19.9 6.91 16.1 4 12 4a7.77 7.77 0 0 0-1.4.12a1 1 0 1 0 .34 2ZM3.71 2.29a1 1 0 0 0-1.42 1.42l3.1 3.09a14.62 14.62 0 0 0-3.31 4.8a1 1 0 0 0 0 .8C4.1 17.09 7.9 20 12 20a9.26 9.26 0 0 0 5.05-1.54l3.24 3.25a1 1 0 0 0 1.42 0a1 1 0 0 0 0-1.42Zm6.36 9.19l2.45 2.45A1.81 1.81 0 0 1 12 14a2 2 0 0 1-2-2a1.81 1.81 0 0 1 .07-.52ZM12 18c-3.18 0-6.17-2.29-7.9-6a12.09 12.09 0 0 1 2.7-3.79L8.57 10A4 4 0 0 0 14 15.43L15.59 17A7.24 7.24 0 0 1 12 18Z"
                  ></path>
                </svg></button
            >
          </div>
          <div id="error-password-message" class="error-message"></div>
        </div>
)rawliteral"; // End of index_html7

// --- Reinstated index_html8 for the code input block ---
static constexpr const char index_html8[] PROGMEM = R"rawliteral(
        <!-- Device Key Input Block -->
        <div class="container" id="codeinputBlock" style="display: none">
          <label id="label-code" for="code"></label> <!-- Label text set by JS -->
          <div id="error-code-input">
            <input
              type="text"
              name="code"
              id="code"
              class="textinput"
              placeholder="" <!-- Placeholder set by JS -->
              aria-label="Device Key"
               maxlength="4" <!-- Maxlength set by JS constant -->
            />
          </div>
          <div id="error-code-message" class="error-message"></div>
        </div>
)rawliteral"; // End of index_html8

// --- index_html9_part1 is now empty, effectively merged into index_html8 ---
// static constexpr const char index_html9_part1[] PROGMEM = R"rawliteral()rawliteral";

// --- Username block ---
static constexpr const char index_html9_username_block[] PROGMEM = R"rawliteral(
        <!-- Service Username Input Block -->
        <div class="container" id="loginFieldsBlock" style="display: none">
          <label id="label-username" for="username"></label> <!-- Label text set by JS -->
          <div id="error-username-input">
            <input
              type="text"
              name="username"
              id="username"
              class="textinput"
              placeholder="" <!-- Placeholder set by JS -->
              aria-label="Service Username"
            />
          </div>
          <div id="error-username-message" class="error-message"></div>
        </div>
)rawliteral";

// --- Service password block ---
static constexpr const char index_html9_service_password_block[] PROGMEM =
    R"rawliteral(
        <!-- Service Password Input Block -->
        <div class="container" id="servicePasswordBlock" style="display: none">
          <label id="label-service-password" for="service_password"></label> <!-- Label text set by JS -->
          <div id="error-service_password-input" class="password-container">
            <input
              type="password"
              name="service_password"
              id="service_password"
              class="textinput"
              placeholder="" <!-- Placeholder set by JS -->
              aria-label="Service Password"
            />
             <button
                class="icon_button"
                type="button"
                title="Show/Hide Service Password"
                onclick="togglePassShow('service_password', 'service_eye_icon');"
              >
                <svg
                  xmlns="http://www.w3.org/2000/svg"
                   style="vertical-align: middle;" width="1em" height="1em"
                  preserveaspectratio="xMidYMid meet"
                  viewbox="0 0 24 24"
                >
                  <path
                    id="service_eye_icon"
                    fill="var(--font-color)"
                    d="M10.94 6.08A6.93 6.93 0 0 1 12 6c3.18 0 6.17 2.29 7.91 6a15.23 15.23 0 0 1-.9 1.64a1 1 0 0 0-.16.55a1 1 0 0 0 1.86.5a15.77 15.77 0 0 0 1.21-2.3a1 1 0 0 0 0-.79C19.9 6.91 16.1 4 12 4a7.77 7.77 0 0 0-1.4.12a1 1 0 1 0 .34 2ZM3.71 2.29a1 1 0 0 0-1.42 1.42l3.1 3.09a14.62 14.62 0 0 0-3.31 4.8a1 1 0 0 0 0 .8C4.1 17.09 7.9 20 12 20a9.26 9.26 0 0 0 5.05-1.54l3.24 3.25a1 1 0 0 0 1.42 0a1 1 0 0 0 0-1.42Zm6.36 9.19l2.45 2.45A1.81 1.81 0 0 1 12 14a2 2 0 0 1-2-2a1.81 1.81 0 0 1 .07-.52ZM12 18c-3.18 0-6.17-2.29-7.9-6a12.09 12.09 0 0 1 2.7-3.79L8.57 10A4 4 0 0 0 14 15.43L15.59 17A7.24 7.24 0 0 1 12 18Z"
                  ></path>
                </svg></button
            >
          </div>
          <div id="error-service_password-message" class="error-message"></div>
        </div>
)rawliteral";

// --- Part 2: Submit button and start of footer ---
static constexpr const char index_html9_part2[] PROGMEM = R"rawliteral(
        <!-- Submit Button -->
        <div class="container">
          <button
            class="btn-process"
            id="submit-btn"
            type="submit"
            form="network_form"
          >
            Connect <span id="connecting-ring" style="display: none"></span>
          </button>
          <div id="error-submit-message" class="error-message"></div>
        </div>
      </form>
    </div>
    
    <!-- Footer -->
    <footer id="footer">
      <p class="copyright" style="opacity: 0.5">
        <a
          role="link"
          style="color: inherit; -webkit-tap-highlight-color: transparent;"
          id="factorylink"
          href="javascript:factoryReset()"
          >Factory Reset</a
        >
      </p>
      <p id="copyright" class="copyright" style="opacity: 0.5"></p> <!-- Footer text injected via JS -->
    </footer>
    
    <!-- JavaScript Section -->
    <script>
      // --- SVG Icons ---
      const svgs = {
        10: `d="m6.67 14.86l3.77 4.7c.8 1 2.32 1 3.12 0l3.78-4.7C17.06 14.65 15.03 13 12 13s-5.06 1.65-5.33 1.86z"/><path fill="var(--font-color)" fill-opacity=".3" d="M23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0L23.64 7z"/></svg>`,
        11: `fill-opacity=".3" d="M15.5 14.5c0-2.8 2.2-5 5-5c.36 0 .71.04 1.05.11L23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0l1.94-2.42V14.5z"/><path fill="var(--font-color)" d="M15.5 14.5c0-.23.04-.46.07-.68c-.92-.43-2.14-.82-3.57-.82c-3 0-5.1 1.7-5.3 1.9l3.74 4.66c.8 1 2.32 1 3.12 0l1.94-2.42V14.5zM23 16v-1.5c0-1.4-1.1-2.5-2.5-2.5S18 13.1 18 14.5V16c-.5 0-1 .5-1 1v4c0 .5.5 1 1 1h5c.5 0 1-.5 1-1v-4c0-.5-.5-1-1-1zm-1 0h-3v-1.5c0-.8.7-1.5 1.5-1.5s1.5.7 1.5 1.5V16z"/></svg>`,
        20: `fill-opacity=".3" d="M23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0L23.64 7z"/><path fill="var(--font-color)" d="m4.79 12.52l5.65 7.04c.8 1 2.32 1 3.12 0l5.65-7.05C18.85 12.24 16.1 10 12 10s-6.85 2.24-7.21 2.52z"/></svg>`,
        21: `fill-opacity=".3" d="M15.5 14.5c0-2.8 2.2-5 5-5c.36 0 .71.04 1.05.11L23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0l1.94-2.42V14.5z"/><path fill="var(--font-color)" d="M15.5 14.5c0-1.34.51-2.53 1.34-3.42C15.62 10.51 13.98 10 12 10c-4.1 0-6.8 2.2-7.2 2.5l5.64 7.05c.8 1 2.32 1 3.12 0l1.94-2.42V14.5zM23 16v-1.5c0-1.4-1.1-2.5-2.5-2.5S18 13.1 18 14.5V16c-.5 0-1 .5-1 1v4c0 .5.5 1 1 1h5c.5 0 1-.5 1-1v-4c0-.5-.5-1-1-1zm-1 0h-3v-1.5c0-.8.7-1.5 1.5-1.5s1.5.7 1.5 1.5V16z"/></svg>`,
        30: `fill-opacity=".3" d="M23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0L23.64 7z"/><path fill="var(--font-color)" d="m3.53 10.95l6.91 8.61c.8 1 2.32 1 3.12 0l6.91-8.61C20.04 10.62 16.81 8 12 8s-8.04 2.62-8.47 2.95z"/></svg>`,
        31: `fill-opacity=".3" d="M15.5 14.5c0-2.8 2.2-5 5-5c.36 0 .71.04 1.05.11L23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0l1.94-2.42V14.5z"/><path fill="var(--font-color)" d="M15.5 14.5a4.92 4.92 0 0 1 3.27-4.68C17.29 8.98 14.94 8 12 8c-4.81 0-8.04 2.62-8.47 2.95l6.91 8.61c.8 1 2.32 1 3.12 0l1.94-2.42V14.5zM23 16v-1.5c0-1.4-1.1-2.5-2.5-2.5S18 13.1 18 14.5V16c-.5 0-1 .5-1 1v4c0 .5.5 1 1 1h5c.5 0 1-.5 1-1v-4c0-.5-.5-1-1-1zm-1 0h-3v-1.5c0-.8.7-1.5 1.5-1.5s1.5.7 1.5 1.5V16z"/></svg>`,
        40: `d="M23.64 7c-.45-.34-4.93-4-11.64-4C5.28 3 .81 6.66.36 7l10.08 12.56c.8 1 2.32 1 3.12 0L23.64 7z"/></svg>`,
        41: `d="M23.21 8.24C20.22 5.6 16.3 4 12 4S3.78 5.6.79 8.24C.35 8.63.32 9.3.73 9.71l5.62 5.63l4.94 4.95c.39.39 1.02.39 1.42 0l2.34-2.34V15c0-.45.09-.88.23-1.29c.54-1.57 2.01-2.71 3.77-2.71h2.94l1.29-1.29c.4-.41.37-1.08-.07-1.47z"/><path fill="var(--font-color)" d="M22 16v-1c0-1.1-.9-2-2-2s-2 .9-2 2v1c-.55 0-1 .45-1 1v3c0 .55.45 1 1 1h4c.55 0 1-.45 1-1v-3c0-.55-.45-1-1-1zm-1 0h-2v-1c0-.55.45-1 1-1s1 .45 1 1v1z"/></svg>`,
      };
      const eye_open = `M21.92 11.6C19.9 6.91 16.1 4 12 4s-7.9 2.91-9.92 7.6a1 1 0 0 0 0 .8C4.1 17.09 7.9 20 12 20s7.9-2.91 9.92-7.6a1 1 0 0 0 0-.8ZM12 18c-3.17 0-6.17-2.29-7.9-6C5.83 8.29 8.83 6 12 6s6.17 2.29 7.9 6c-1.73 3.71-4.73 6-7.9 6Zm0-10a4 4 0 1 0 4 4a4 4 0 0 0-4-4Zm0 6a2 2 0 1 1 2-2a2 2 0 0 1-2 2Z`;
      const eye_close = `M10.94 6.08A6.93 6.93 0 0 1 12 6c3.18 0 6.17 2.29 7.91 6a15.23 15.23 0 0 1-.9 1.64a1 1 0 0 0-.16.55a1 1 0 0 0 1.86.5a15.77 15.77 0 0 0 1.21-2.3a1 1 0 0 0 0-.79C19.9 6.91 16.1 4 12 4a7.77 7.77 0 0 0-1.4.12a1 1 0 1 0 .34 2ZM3.71 2.29a1 1 0 0 0-1.42 1.42l3.1 3.09a14.62 14.62 0 0 0-3.31 4.8a1 1 0 0 0 0 .8C4.1 17.09 7.9 20 12 20a9.26 9.26 0 0 0 5.05-1.54l3.24 3.25a1 1 0 0 0 1.42 0a1 1 0 0 0 0-1.42Zm6.36 9.19l2.45 2.45A1.81 1.81 0 0 1 12 14a2 2 0 0 1-2-2a1.81 1.81 0 0 1 .07-.52ZM12 18c-3.18 0-6.17-2.29-7.9-6a12.09 12.09 0 0 1 2.7-3.79L8.57 10A4 4 0 0 0 14 15.43L15.59 17A7.24 7.24 0 0 1 12 18Z`;

      // --- DOM Element References ---
      const table = document.getElementById("table-body");
      const form = document.getElementById("network_form");
      const code_listener = document.getElementById("code");
      const ssid_listener = document.getElementById("ssid"); // Hidden SSID input
      const password_listener = document.getElementById("password"); // Network password input
      const username_listener = document.getElementById("username"); // Service username input
      const service_password_listener = document.getElementById("service_password"); // Service password input

      // --- Add Event Listeners ---
      if(form) form.addEventListener("submit", submitForm);
      if(code_listener) code_listener.addEventListener("input", updateValue);
      if(ssid_listener) ssid_listener.addEventListener("input", updateValue);
      if(password_listener) password_listener.addEventListener("input", updateValue);
      if(username_listener) username_listener.addEventListener("input", updateValue);
      if(service_password_listener) service_password_listener.addEventListener("input", updateValue);

      // --- Injected Constants (Values are injected by C++ code here) ---
)rawliteral";
// Config: PROJECT_TITLE injected here
// Merged into index_html4
// static constexpr const char index_html5[] PROGMEM = R"rawliteral(`;)rawliteral";
// Config: PROJECT_SUB_TITLE injected here
// Merged into index_html4
// static constexpr const char index_html6[] PROGMEM = R"rawliteral(`;)rawliteral";
// Config: PROJECT_INFO injected here
// Merged into index_html4
// static constexpr const char index_html7[] PROGMEM = R"rawliteral(`;)rawliteral";

// --- JavaScript constants split for injection ---
static constexpr const char js_const_part1[] PROGMEM = R"rawliteral(
      const title_logo = `)rawliteral";
// Config: SVG_LOGO injected here (duplicate for JS)
static constexpr const char js_const_part1a[] PROGMEM = R"rawliteral(`; const title_text = `)rawliteral";
// Config: PROJECT_TITLE injected here (duplicate for JS)
static constexpr const char js_const_part1b[] PROGMEM = R"rawliteral(`; const title_sub = `)rawliteral";
// Config: PROJECT_SUB_TITLE injected here (duplicate for JS)
static constexpr const char js_const_part1c[] PROGMEM = R"rawliteral(`; const title_info = `)rawliteral";
// Config: PROJECT_INFO injected here (duplicate for JS)

static constexpr const char js_const_part1d[] PROGMEM = R"rawliteral(`; const input_name_text = `)rawliteral";
// Config: INPUT_TEXT injected here
static constexpr const char js_const_part2[] PROGMEM = R"rawliteral(`; const input_lenght = )rawliteral";
// Config: INPUT_LENGTH injected here (as number)
static constexpr const char js_const_part3[] PROGMEM = R"rawliteral(; const connection_successful_text = `)rawliteral";
// Config: CONNECTION_SUCCESSFUL injected here
static constexpr const char js_const_part4[] PROGMEM = R"rawliteral(`; const footer_text = `)rawliteral";
// Config: FOOTER_TEXT injected here
static constexpr const char js_const_part5[] PROGMEM = R"rawliteral(`; const reset_confirmation_text = `)rawliteral";
// Config: RESET_CONFIRMATION_TEXT injected here
static constexpr const char js_const_part6[] PROGMEM = R"rawliteral(`; const username_text = `)rawliteral";
// Config: USERNAME_TEXT injected here
static constexpr const char js_const_part7[] PROGMEM = R"rawliteral(`; const service_password_text = `)rawliteral";
// Config: SERVICE_PASSWORD_TEXT injected here
static constexpr const char js_const_part8[] PROGMEM = R"rawliteral(`; const show_input_field = )rawliteral"; // Added show_input_field here
// Config: SHOW_INPUT_FIELD injected here (as boolean literal 'true' or 'false')
static constexpr const char js_const_part8a[] PROGMEM = R"rawliteral(; const show_login_fields = )rawliteral";
// Config: SHOW_LOGIN_FIELDS injected here (as boolean literal 'true' or 'false')
static constexpr const char js_const_part9[] PROGMEM = R"rawliteral(; const reset_show = )rawliteral";
// Config: SHOW_RESET_FIELD injected here (as boolean literal 'true' or 'false')

// --- End of JS const definition; rest of HTML/JS follows ---
static constexpr const char index_html13[] PROGMEM = R"rawliteral(
;
      
      // --- Initialization on DOM ready ---
      window.addEventListener("DOMContentLoaded", (event) => {
          
         // Set theme color RGB variables for focus ring (parse from hex)
         try {
             const style = getComputedStyle(document.documentElement);
             const hex = style.getPropertyValue('--theme-color').trim();
             if (hex && hex.startsWith('#') && (hex.length === 7 || hex.length === 4)) {
                const fullHex = hex.length === 4 ? `#${hex[1]}${hex[1]}${hex[2]}${hex[2]}${hex[3]}${hex[3]}` : hex;
                const r = parseInt(fullHex.substring(1, 3), 16);
                const g = parseInt(fullHex.substring(3, 5), 16);
                const b = parseInt(fullHex.substring(5, 7), 16);
                document.documentElement.style.setProperty('--theme-color-r', r);
                document.documentElement.style.setProperty('--theme-color-g', g);
                document.documentElement.style.setProperty('--theme-color-b', b);
             } else {
                 console.warn("Could not parse --theme-color for focus ring:", hex);
             }
         } catch(e) { console.error("Error setting theme color RGB variables:", e); }


        // Use JS constants defined above to set text content, placeholders etc.
        const logoContainer = document.getElementById("logo-container");
        const appTitle = document.getElementById("app-title");
        const appSubtitle = document.getElementById("app-subtitle");
        const infoText = document.getElementById("info-text");
        const codeInput = document.getElementById("code");
        const codeLabel = document.getElementById("label-code");
        const userLabel = document.getElementById("label-username");
        const userInput = document.getElementById("username");
        const servicePassLabel = document.getElementById("label-service-password");
        const servicePassInput = document.getElementById("service_password");
        const copyrightText = document.getElementById("copyright");
        const resetLink = document.getElementById("factorylink");

        if(logoContainer) logoContainer.innerHTML = title_logo;
        if(appTitle) appTitle.textContent = title_text;
        if(appSubtitle) appSubtitle.textContent = title_sub;
        if(infoText) infoText.textContent = title_info;

         if(codeInput) {
             codeInput.setAttribute("maxlength", input_lenght);
             codeInput.placeholder = input_name_text;
         }
         if(codeLabel) codeLabel.textContent = input_name_text;

         if(userLabel) userLabel.textContent = username_text;
         if(userInput) userInput.placeholder = username_text;

         if(servicePassLabel) servicePassLabel.textContent = service_password_text;
         if(servicePassInput) servicePassInput.placeholder = service_password_text;


        if(copyrightText) copyrightText.textContent = footer_text;

        if (!reset_show && resetLink) {
          resetLink.style.display = "none";
        }
      });

      // --- Load networks on page load ---
      window.addEventListener("load", (event) => {
        loadSSID(); // Initial network scan on load
      });

     // --- (Rest of JavaScript functions are unchanged) ---
     
      function updateValue(e) {
        // Clear error message for the input field being typed in
        showError(e.target.id, "", false);
      }

      function isRadioChecked() {
        // Check if any network radio button (including hidden) is checked
        return Array.from(
          document.querySelectorAll('input[name="ssid"]')
        ).some((radio) => radio.checked);
      }

      function showError(name, message, state) {
        // Show/hide error message and styling for a given input field name
         // Use lowercase name consistently
        const fieldName = name ? name.toLowerCase() : '';
        if (!fieldName) return; // Exit if name is invalid
        
        // Find input block and message element by convention
        const block = document.getElementById(`error-${fieldName}-input`);
        const messageElem = document.getElementById(`error-${fieldName}-message`);

        // Toggle error class on the container for styling input
        // Check if block exists before trying to access classList
        if (block) {
             block.classList.toggle("error", state);
         } else if (fieldName === 'submit') {
             // Handle submit error display differently if needed, e.g., show it directly
             const submitErrorElem = document.getElementById('error-submit-message');
             if(submitErrorElem) submitErrorElem.textContent = state ? message : "";
         }

        // Set error message text
        if (messageElem) messageElem.textContent = state ? message : "";
      }


      function resetErrors() {
        // Clear errors for all potentially visible fields
        ["submit", "code", "password", "ssid", "username", "service_password"].forEach((field) => {
          showError(field, "", false);
        });
      }

      function connectingState(state) {
        // Update submit button text and state (disabled/enabled) + spinner visibility
        const ring = document.getElementById("connecting-ring");
        const submitBtn = document.getElementById("submit-btn");

        if (!submitBtn) { // Safety check
          return;
        }

        const buttonTxt = state ? "Connecting" : "Connect";
        // Update spinner visibility directly
        if(ring) ring.style.display = state ? "inline-block" : "none"; // Use inline-block
        
        // Set button text content without overwriting the span
        let textNode = Array.from(submitBtn.childNodes).find(node => node.nodeType === Node.TEXT_NODE && node.textContent.trim() !== '');
        if (textNode) {
            textNode.textContent = buttonTxt + (state ? ' ' : ''); // Add space before spinner
        } else {
             // If no text node, prepend it
             submitBtn.insertBefore(document.createTextNode(buttonTxt + (state ? ' ' : '')), submitBtn.firstChild);
        }

        submitBtn.disabled = state;
      }


      function submitForm(event) {
        event.preventDefault(); // Prevent default form submission

        // --- Validation ---
        const validations = [
          // 1. Check if a network is selected (either listed or hidden)
          {
            condition: !isRadioChecked(),
            field: "submit", // General form error
            message: "Please select a WiFi network or specify a hidden one.",
          },
          // 2. Validate 'Device Key' if it's visible AND configured to be shown
           {
            condition: isCodeVisible() && show_input_field, // Check both visibility and config flag
            field: "code",
            message: (() => {
                const value = code_listener ? code_listener.value.trim() : '';
                // Only require if field is configured to be shown (show_input_field)
                if (!value && show_input_field) {
                    return `${input_name_text} is required.`;
                }
                // Check length only if there is a value
                if (value && value.length > input_lenght) {
                    return `${input_name_text} cannot exceed ${input_lenght} characters.`;
                }
                return null; // No error
            })(),
           },
           // 3. Validate 'Username' if login fields are visible AND configured to be shown
          {
            condition: isLoginVisible() && show_login_fields && (!username_listener || !username_listener.value.trim()),
            field: "username",
            message: `${username_text} is required.`,
          },
           // 4. Validate 'Service Password' if login fields are visible AND configured to be shown
          {
            condition: isLoginVisible() && show_login_fields && (!service_password_listener || !service_password_listener.value),
            field: "service_password",
            message: `${service_password_text} is required.`,
          },
          // 5. Validate 'Hidden SSID' if hidden network is selected
          {
            condition: isHidden() && (!ssid_listener || !ssid_listener.value.trim()),
            field: "ssid", // ID of the hidden SSID input
            message: "Hidden Network SSID is required.",
          },
          // 6. Validate 'Network Password' if required (selected network is secured OR hidden network is selected)
          {
             condition: (isAuth() || isHidden()) && (!password_listener || !password_listener.value), // Don't trim password
             field: "password", // ID of the network password input
             message: "Network Password is required.",
          },
        ];

        let hasErrors = false;
        resetErrors(); // Clear previous errors first

        validations.forEach(({ condition, field, message }) => {
           // Only validate if the message function returns a non-null message
           const errorMessage = typeof message === 'function' ? message() : message;
          if (condition && errorMessage) {
            showError(field, errorMessage, true);
            hasErrors = true;
          }
        });

        if (hasErrors) return; // Stop if validation failed

        // --- If validation passes ---
        connectingState(true); // Show connecting status
        disableForm(true); // Disable form elements

        // --- Prepare Payload ---
        let payload = {};

        // Get SSID (either selected radio or hidden input)
        if (isHidden()) {
          payload.ssid = ssid_listener ? ssid_listener.value.trim() : '';
        } else {
          const selectedRadio = document.querySelector(
            'input[name="ssid"]:checked' // Ensure it's checked
          );
          if (selectedRadio) { // Should always be true if validation passed
              payload.ssid = selectedRadio.value;
          } else {
             // Fallback/error - should not happen if validation is correct
             console.error("No SSID selected despite passing validation");
             showError("submit", "Internal Error: No SSID selected.", true);
             connectingState(false);
             disableForm(false);
             return;
          }
        }


        // Get Network Password if needed
        if ((isAuth() || isHidden()) && password_listener && password_listener.value) {
          payload.password = password_listener.value;
        }

        // Get Device Key if visible AND configured to be shown
        if (isCodeVisible() && show_input_field && code_listener) {
          payload.code = code_listener.value.trim(); // Trim device key if needed
        }
        
        // Get Login Credentials if visible AND configured to be shown
        if (isLoginVisible() && show_login_fields) {
           payload.username = username_listener ? username_listener.value.trim() : ''; // Trim username
           payload.service_password = service_password_listener ? service_password_listener.value : ''; // Don't trim password
        }

        // --- Send Request ---
        console.log("Sending payload:", JSON.stringify(payload)); // Log for debugging
        fetch("/configure", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(payload),
        })
          .then((response) => {
             if (!response.ok) {
                 // Try to get error text if available
                 return response.text().then(text => {
                      console.error("Configure request failed:", response.status, text);
                     throw new Error(`Connection request failed: ${response.status}`);
                 });
             }
            return response.json();
          })
          .then((jsonResponse) => {
            console.log("Received response:", jsonResponse); // Log for debugging
            if (jsonResponse.success) {
              successPage(payload.ssid); // Show success page
            } else {
              // Handle specific errors based on 'reason'
              const reason = jsonResponse.reason || 'Unknown';
              console.warn("Connection failed. Reason:", reason);

              if (reason === "code") {
                showError("code", `Invalid ${input_name_text}`, true);
              } else if (reason === "ssid") {
                 // Could be wrong password or SSID unreachable/doesn't exist
                 if (isAuth() || isHidden()) { // Check if password was expected
                    showError("password", "Invalid password or cannot connect.", true);
                 } else {
                     showError("submit", `Could not connect to '${payload.ssid}'. Network may be out of range.`, true);
                 }
              } else if (reason === "login") { // Handle potential login failure reason
                  showError("username", "Invalid service username or password.", true); // Show general login error
                  showError("service_password", "Invalid service username or password.", true);
              }
               else {
                // Generic error
                showError( "submit", `Connection failed. Please check credentials and try again.`, true);
              }
               connectingState(false); // Re-enable form on error
               disableForm(false);
            }
          })
          .catch((error) => {
              console.error("Fetch error during configure:", error);
              showError("submit", `Error during connection. Please check device logs.`, true);
              connectingState(false); // Re-enable form on fetch error
              disableForm(false);
           });
      }


      function successPage(ssid_text) {
        // Replace card content with success message and checkmark animation
        const card = document.getElementById("main-card");
        if (!card) return; // Safety check
        card.innerHTML = `
       <div class="wrapper">
          <svg class="checkmark" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 52 52"> <circle class="checkmark__circle" cx="26" cy="26" r="25" fill="none"/> <path class="checkmark__check" fill="none" d="M14.1 27.2l7.1 7.2 16.7-16.8"/></svg>
       </div>
       <div class="container" style="padding: 1rem;">
         <h2 style="color:#7ac142;word-break: break-word;">Success</h2>
         <p style="color:#7ac142;word-break: break-word;font-size:1.2rem;margin-bottom: 0.5rem;">Successfully connected to</p>
         <p style="color:#7ac142;word-break: break-word;margin-top: 0rem; font-weight: bold;">${ssid_text}</p>
         <p style="opacity: 0.5; margin-top: 1rem;">${connection_successful_text}</p>
         <p style="opacity: 0.5;">You can close this window now.</p>
       </div>
       `;
        // Optionally hide the footer or other elements
        const footer = document.getElementById("footer");
        if(footer) footer.style.display = 'none';
      }


      function onRadio(element) {
        // When a radio button is clicked (either listed SSID or 'Hidden Network')
        if (!element) return;
        resetErrors(); // Clear any existing errors

        const isHiddenNetwork = element.id === "hiddennetwork_radio";
        const isListedNetwork = !isHiddenNetwork;

        // Show/hide the input field for the hidden SSID name
        showssidField(isHiddenNetwork);

        // Determine if the selected network requires a password
        let needsPassword = false;
        if (isHiddenNetwork) {
            needsPassword = true; // Assume hidden networks always need a password for simplicity
        } else if (isListedNetwork) {
            // Check the data-auth attribute of the selected listed network
            needsPassword = element.dataset.auth && parseInt(element.dataset.auth) > 0;
        }
        
        // Show/hide the network password input field
        showpasswordField(needsPassword);

        // Uncheck other radio buttons if 'Hidden Network' is selected
        if (isHiddenNetwork) {
          const radios = document.querySelectorAll(
            'input[name="ssid"]:not(#hiddennetwork_radio)'
          );
          radios.forEach((radio) => (radio.checked = false));
        } else {
             // If a listed network is selected, uncheck 'Hidden Network'
             const hiddenRadio = document.getElementById("hiddennetwork_radio");
             if(hiddenRadio) hiddenRadio.checked = false;
        }
      }


      function disableForm(state) {
        // Disable or enable all form elements (inputs, buttons)
        if (!form) return;
        Array.from(form.elements).forEach((el) => (el.disabled = state));
        // Also disable/enable the factory reset link
        disableLinks(state);
      }

      function refreshSpin(state) {
         // Show/hide the spinning animation on the refresh button
        const icon = document.getElementById("refresh-icon");
        if(icon) icon.classList.toggle("icn-spinner", state);
      }

      function loadSSID() {
        console.log("loadSSID called - fetching /update"); // Debug log
        disableForm(true); // Disable form during scan
        refreshSpin(true); // Show spinner
        resetErrors(); // Clear errors
        
         // Clear existing table rows and show scanning message
         if(table) table.innerHTML = '<tr><td colspan="3" style="text-align: center; color: grey;">Scanning for networks...</td></tr>';

        // Add cache buster to prevent browser caching of the result
        const url = "/update?t=" + Date.now(); 

        fetch(url) // Use the URL with cache buster
          .then((response) => {
            console.log("Fetch response status:", response.status); // Debug log
            if (!response.ok) {
                // Log response text for more details on failure
                 return response.text().then(text => {
                     console.error(`Network scan request failed: ${response.status} ${response.statusText}`, text);
                     throw new Error(`Network scan failed (${response.status})`);
                 });
             }
            return response.json(); // Attempt to parse JSON
          })
          .then((jsonResponse) => {
             console.log("Received network data JSON:", jsonResponse); // Debug log
             if (!table) return; // Safety check
             // Clear existing table rows again
             table.innerHTML = ''; 

            // Check if network data is valid array
            if (jsonResponse.network && Array.isArray(jsonResponse.network) && jsonResponse.network.length > 0) {
                 console.log(`Processing ${jsonResponse.network.length} networks...`); // Debug log
                 jsonResponse.network.forEach(({ ssid, authmode, rssi }) => {
                     // Basic validation of data before adding row
                     if (typeof ssid === 'string' && typeof authmode === 'number' && typeof rssi === 'number') {
                         addTableRow(ssid, authmode, rssi); // Add row for each network
                     } else {
                         console.warn("Received invalid network data item:", { ssid, authmode, rssi });
                     }
                 });
                 console.log("Finished processing networks."); // Debug log
             } else {
                 console.log("No networks found or network data is empty/invalid.");
                 table.innerHTML = '<tr><td colspan="3" style="text-align: center; color: grey;">No WiFi networks found. Try refreshing.</td></tr>';
             }
             
            // Show/hide fields based on JSON response (use injected boolean constants for logic elsewhere)
            // Use !! to ensure boolean conversion
            showcodeField(!!jsonResponse.show_code); 
            showLoginFields(!!jsonResponse.show_login);
            console.log("showcodeField called with:", !!jsonResponse.show_code); // Debug
            console.log("showLoginFields called with:", !!jsonResponse.show_login); // Debug
          })
          .catch((error) => {
            console.error("Error during network scan fetch/processing:", error); // More specific error log
            showError("submit", `Error fetching networks. Please refresh.`, true);
            if(table) table.innerHTML = '<tr><td colspan="3" style="text-align: center; color: red;">Error loading networks. Refresh or check device.</td></tr>'; // Show error in table
          })
          .finally(() => {
            refreshSpin(false); // Hide spinner
            disableForm(false); // Re-enable form
             console.log("loadSSID finished"); // Debug log
          });
      }


      function addTableRow(ssid, authmode, rssi) {
        // Add a row to the network table
         if (!table) return; // Safety check
        const locked = authmode > 0 ? 1 : 0; // Use 1 for locked, 0 for open
         // Ensure RSSI level is within 0-4 range expected by svgs object keys
         const safeSignalLevel = Math.max(0, Math.min(4, rssi || 0)); // Default to 0 if rssi is undefined/null
        const iconKey = `${safeSignalLevel}${locked}`;
        const iconPath = svgs[iconKey]; // Get the correct SVG path

         let iconHtml = '<!-- Icon N/A -->'; // Default if icon not found
         if (iconPath) {
             // Note: iconPath already includes the closing </svg> tag in the svgs object
             iconHtml = `<svg xmlns="http://www.w3.org/2000/svg" style="vertical-align: middle;" width="1.2em" height="1.2em" preserveAspectRatio="xMidYMid meet" viewBox="0 0 24 24"><path fill="var(--font-color)" ${iconPath}`;
         } else {
              console.warn(`No SVG found for signal key ${iconKey} (RSSI level ${safeSignalLevel}, locked: ${locked})`);
         }


        // Use textContent for SSID to prevent potential XSS if SSID contains HTML
        const safeSSID = document.createElement('span');
        safeSSID.textContent = ssid;
         // Create a safer ID by replacing non-alphanumeric chars with underscores + random suffix
         const uniqueId = `ssid-${ssid.replace(/[^a-zA-Z0-9]/g, '_')}-${Math.random().toString(36).substring(2, 7)}`;


        table.innerHTML += `
       <tr>
           <td class="radiossid">
             <input type="radio" name="ssid" value="${ssid}" data-auth="${authmode}" onclick="onRadio(this)" id="${uniqueId}" class="circle">
           </td>
            <td><label for="${uniqueId}">${safeSSID.innerHTML}</label></td>
           <td class="signal">${iconHtml}</td>
       </tr>
       `;
      }

       // Updated function to handle multiple password fields
      function togglePassShow(inputId, iconId) {
          const input = document.getElementById(inputId);
          const iconPath = document.getElementById(iconId); // Get the path element
          if (!input || !iconPath) {
             console.warn("Could not find input or icon element for togglePassShow:", inputId, iconId);
             return;
          }

          const isPassword = input.type === "password";
          input.type = isPassword ? "text" : "password";
          iconPath.setAttribute("d", isPassword ? eye_open : eye_close); // Set the 'd' attribute of the path
      }


      function showssidField(state) {
          // Show/hide hidden SSID input block
           const elem = document.getElementById("hiddenNetwork");
           if(elem) elem.style.display = state ? "" : "none";
      }

      function showpasswordField(state) {
         // Show/hide network password input block
          const elem = document.getElementById("hiddenPassword");
           if(elem) elem.style.display = state ? "" : "none";
      }

      function showcodeField(state) {
        // Show/hide device key input block
          const elem = document.getElementById("codeinputBlock");
           if(elem) elem.style.display = state ? "" : "none";
           console.log("codeinputBlock display set to:", state ? "" : "none"); // Debug
      }

      function showLoginFields(state) {
         // Show/hide both username and service password blocks
        const userBlock = document.getElementById("loginFieldsBlock");
        const passBlock = document.getElementById("servicePasswordBlock");
         if(userBlock) userBlock.style.display = state ? "" : "none";
         if(passBlock) passBlock.style.display = state ? "" : "none";
         console.log("loginFieldsBlock display set to:", state ? "" : "none"); // Debug
      }

      function isLoginVisible() {
         // Check if the username block is visible (implies both login fields are)
         const userBlock = document.getElementById("loginFieldsBlock");
        return userBlock ? userBlock.style.display !== "none" : false;
      }


      function isHidden() {
          // Check if the 'Hidden Network' radio is selected
          const hiddenRadio = document.getElementById("hiddennetwork_radio");
          return hiddenRadio ? hiddenRadio.checked : false;
      }

      function isAuth() {
        // Check if the currently selected *listed* network requires authentication
        const selectedRadio = document.querySelector(
          'input[name="ssid"]:checked:not(#hiddennetwork_radio)' // Exclude hidden radio
        );
        // Check data-auth attribute (0 = open, >0 = secured)
        return selectedRadio ? parseInt(selectedRadio.dataset.auth || "0") > 0 : false;
      }

      function isCodeVisible() {
          // Check if device key input block is visible
          const codeBlock = document.getElementById("codeinputBlock");
          return codeBlock ? codeBlock.style.display !== "none" : false;
      }
      function isPasswordVisible() {
         // Check if network password input block is visible
          const passBlock = document.getElementById("hiddenPassword");
          return passBlock ? passBlock.style.display !== "none" : false;
      }
      function isSsidVisible() {
         // Check if hidden SSID input block is visible
          const ssidBlock = document.getElementById("hiddenNetwork");
          return ssidBlock ? ssidBlock.style.display !== "none" : false;
      }

      function disableLinks(state) {
         // Disable/enable the factory reset link
        const link = document.getElementById("factorylink");
         if(link) link.style.pointerEvents = state ? "none" : "";
         if(link) link.style.opacity = state ? "0.5" : "1"; // Visually fade when disabled
      }


      function factoryReset() {
        // Show confirmation dialog for factory reset
        const card = document.getElementById("main-card");
         if (!card) return; // Safety check
        card.innerHTML = `
               <div id="factory-reset-section">
                 <h3 style="padding-top: 1rem; margin: 0rem;">Factory Reset</h3>
                 <p>Are you sure you want to reset the device to factory settings?</p>
                 <p style="color: gray; margin-top: 1rem;">${reset_confirmation_text}</p>
                 <div style="display: flex; justify-content: center; gap: 1rem; margin-top: 1.5rem;">
                    <button
                      id="cancelReset"
                      class="btn-process"
                      style="background-color: gray;"
                      onclick="cancelFactoryReset();"
                      >
                      Cancel <span id="cancel-spinner" style="display: none;"></span>
                    </button>
                    <button
                      id="resetYes"
                      class="btn-process"
                      style="background-color: #cc0033;" /* Red color for reset */
                      onclick="startFactoryReset();"
                      >
                      Reset <span id="connecting-ring" style="display: none;"></span>
                    </button>
                 </div>
                 <p id="reset-feedback" class="error-message"></p>
               </div>
             `;
             // Hide footer during confirmation
              const footer = document.getElementById("footer");
              if(footer) footer.style.display = 'none';
      }

      function disableFactoryResetButtons(state) {
          // Disable/enable reset confirmation buttons
          const yesButton = document.getElementById("resetYes");
          const noButton = document.getElementById("cancelReset");
          if(yesButton) yesButton.disabled = state;
          if(noButton) noButton.disabled = state;
           disableLinks(state); // Also disable link if needed
      }


      function startFactoryReset() {
        disableFactoryResetButtons(true); // Disable buttons

        const resetButton = document.getElementById("resetYes");
        const factoryResetSection = document.getElementById("factory-reset-section");
        const feedback = document.getElementById("reset-feedback");

        if(!resetButton || !factoryResetSection || !feedback) return; // Safety check

        // Show spinner on reset button
        resetButton.innerHTML = `Resetting<span id="connecting-ring" style="display: inline-block;"></span>`;
        feedback.textContent = ""; // Clear previous feedback

        fetch("/factoryreset", { method: "POST" })
          .then((response) => {
             if (!response.ok) {
                 // Try to get error text if available
                 return response.text().then(text => {
                      console.error("Factory reset request failed:", response.status, text);
                     throw new Error(`Reset request failed: ${response.status}`);
                 });
             }
             // Don't expect JSON, just success status
             return response.text(); // Consume body
          })
          .then(() => {
                // Show success message within the existing structure
               factoryResetSection.innerHTML = `
                 <div class="wrapper">
                   <svg class="checkmark" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 52 52">
                     <circle class="checkmark__circle" cx="26" cy="26" r="25" fill="none" />
                     <path class="checkmark__check" fill="none" d="M14.1 27.2l7.1 7.2 16.7-16.8" />
                   </svg>
                 </div>
                 <div class="container" style="padding: 1rem;">
                   <h2 style="color:#7ac142;word-break: break-word;">Reset Success</h2>
                   <p style="color: gray;word-break: break-word;font-size:1.2rem;margin-bottom: 0.5rem;">Factory reset successful.</p>
                   <p style="opacity: 0.5;">Device will restart. Reconnect to AP to configure again.</p>
                 </div>
               `;
               // No automatic redirect, user needs to reconnect after ESP32 restart (implied)
          })
          .catch((error) => {
            console.error("Factory reset error:", error);
            feedback.textContent = `Error during reset. Check device logs.`;
            disableFactoryResetButtons(false); // Re-enable buttons on error
             if(resetButton) resetButton.innerHTML = 'Reset <span id="connecting-ring" style="display: none;"></span>'; // Reset button text
          });
      }

      function cancelFactoryReset() {
        // Simply reload the page to cancel the reset confirmation
        disableFactoryResetButtons(true); // Disable buttons while reloading
        const spinner = document.getElementById("cancel-spinner");
        if(spinner) spinner.style.display = "inline-block"; // Show spinner on cancel

        // Add a small delay before reloading to show spinner
        setTimeout(() => {
             window.location.href = "/"; // Reload the root page
        }, 300);
      }
    </script>
  </body>
</html>
)rawliteral";

#endif // PROVISION_HTML_H

