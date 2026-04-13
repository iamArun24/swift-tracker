import { useCallback, useEffect, useMemo, useRef, useState } from "react";
import gsap from "gsap";

const LABELS = {
  idle: "Create Shipment",
  loading: "Dispatching...",
  success: "Shipment Created",
  error: "Try Again",
};

const SCOOTER_END_X = 110;

function wait(ms) {
  return new Promise((resolve) => {
    window.setTimeout(resolve, ms);
  });
}

function timelineDone(tl) {
  return new Promise((resolve) => {
    const prevOnComplete = tl.eventCallback("onComplete");
    tl.eventCallback("onComplete", (...args) => {
      if (typeof prevOnComplete === "function") {
        prevOnComplete.apply(tl, args);
      }
      resolve();
    });
  });
}

export default function ShipmentActionButton({
  onAction,
  disabled = false,
  className = "",
}) {
  const [phase, setPhase] = useState("idle");

  const rootRef = useRef(null);
  const sceneRef = useRef(null);
  const rippleRef = useRef(null);
  const routeRef = useRef(null);
  const progressRef = useRef(null);
  const pinRef = useRef(null);
  const scooterRef = useRef(null);
  const wheelRearRef = useRef(null);
  const wheelFrontRef = useRef(null);
  const parcelRef = useRef(null);
  const checkRef = useRef(null);

  const mountedRef = useRef(false);
  const idleTweensRef = useRef([]);
  const activeTlRef = useRef(null);

  const isBusy = disabled || phase === "loading";

  const label = useMemo(() => LABELS[phase] || LABELS.idle, [phase]);

  const pauseIdle = useCallback(() => {
    idleTweensRef.current.forEach((tween) => tween.pause());
  }, []);

  const resumeIdle = useCallback(() => {
    idleTweensRef.current.forEach((tween) => tween.resume());
  }, []);

  const clearActiveTimeline = useCallback(() => {
    if (activeTlRef.current) {
      activeTlRef.current.kill();
      activeTlRef.current = null;
    }
  }, []);

  const resetScene = useCallback(() => {
    gsap.set(sceneRef.current, { x: 0, y: 0 });
    gsap.set(scooterRef.current, { x: 0, y: 0, rotate: 0, autoAlpha: 1, transformOrigin: "46% 66%" });
    gsap.set(progressRef.current, { autoAlpha: 0.9, scaleX: 0, transformOrigin: "0% 50%" });
    gsap.set(parcelRef.current, { autoAlpha: 0, scale: 0.75, y: 4 });
    gsap.set(checkRef.current, { autoAlpha: 0, scale: 0.8, transformOrigin: "50% 50%" });
  }, []);

  useEffect(() => {
    mountedRef.current = true;

    const ctx = gsap.context(() => {
      resetScene();

      const idleScooter = gsap.to(scooterRef.current, {
        y: -1.5,
        x: 2.4,
        rotate: -0.8,
        yoyo: true,
        repeat: -1,
        duration: 1,
        ease: "sine.inOut",
      });

      const idleWheels = gsap.to([wheelRearRef.current, wheelFrontRef.current], {
        rotate: "+=360",
        transformOrigin: "50% 50%",
        ease: "none",
        duration: 1.4,
        repeat: -1,
      });

      const idleRoute = gsap.to(routeRef.current, {
        strokeDashoffset: -20,
        ease: "none",
        duration: 0.8,
        repeat: -1,
      });

      const idlePin = gsap.to(pinRef.current, {
        scale: 1.08,
        transformOrigin: "50% 100%",
        yoyo: true,
        repeat: -1,
        duration: 0.8,
        ease: "sine.inOut",
      });

      idleTweensRef.current = [idleScooter, idleWheels, idleRoute, idlePin];
    }, rootRef);

    return () => {
      mountedRef.current = false;
      clearActiveTimeline();
      idleTweensRef.current.forEach((tween) => tween.kill());
      idleTweensRef.current = [];
      ctx.revert();
    };
  }, [clearActiveTimeline, resetScene]);

  const animateRipple = useCallback((event) => {
    const root = rootRef.current;
    const ripple = rippleRef.current;

    if (!root || !ripple) {
      return;
    }

    const rect = root.getBoundingClientRect();
    const localX = event.clientX - rect.left;
    const localY = event.clientY - rect.top;

    gsap.killTweensOf(ripple);
    gsap.set(ripple, { x: localX, y: localY, autoAlpha: 0.42, scale: 0.1 });
    gsap.to(ripple, {
      scale: 18,
      autoAlpha: 0,
      duration: 0.7,
      ease: "power2.out",
    });
  }, []);

  const runDispatchTimeline = useCallback(() => {
    clearActiveTimeline();
    pauseIdle();

    // Story: pickup -> route progress -> delivery point reached.
    const tl = gsap.timeline();

    tl.set(checkRef.current, { autoAlpha: 0, scale: 0.8 })
      .set(parcelRef.current, { autoAlpha: 0, y: 4, scale: 0.75 })
      .set(progressRef.current, { autoAlpha: 0.92, scaleX: 0, transformOrigin: "0% 50%" })
      .to(progressRef.current, {
        scaleX: 1,
        transformOrigin: "0% 50%",
        duration: 1.2,
        ease: "power2.inOut",
      }, 0)
      .to(scooterRef.current, {
        x: SCOOTER_END_X,
        duration: 1.18,
        ease: "power1.inOut",
      }, 0.04)
      .to([wheelRearRef.current, wheelFrontRef.current], {
        rotate: "+=880",
        transformOrigin: "50% 50%",
        duration: 1.18,
        ease: "none",
      }, 0.04)
      .fromTo(parcelRef.current,
        { autoAlpha: 0, y: 5, scale: 0.7 },
        { autoAlpha: 1, y: 0, scale: 1, duration: 0.3, ease: "back.out(2.8)" },
        0.9
      )
      .to(scooterRef.current, {
        y: -2,
        duration: 0.18,
        yoyo: true,
        repeat: 1,
        ease: "power1.inOut",
      }, 1.08);

    activeTlRef.current = tl;
    return tl;
  }, [clearActiveTimeline, pauseIdle]);

  const runSuccessTimeline = useCallback(() => {
    clearActiveTimeline();

    const tl = gsap.timeline();

    tl.to(checkRef.current, {
      autoAlpha: 1,
      scale: 1,
      duration: 0.25,
      ease: "back.out(3)",
    }, 0)
      .to(pinRef.current, {
        scale: 1.18,
        duration: 0.2,
        yoyo: true,
        repeat: 1,
        transformOrigin: "50% 100%",
        ease: "power1.inOut",
      }, 0)
      .to(parcelRef.current, {
        y: -1,
        duration: 0.13,
        yoyo: true,
        repeat: 1,
        ease: "power1.inOut",
      }, 0.15)
      .to(scooterRef.current, {
        x: SCOOTER_END_X + 8,
        autoAlpha: 0,
        duration: 0.28,
        ease: "power2.in",
      }, 0.46)
      .set(scooterRef.current, {
        x: -18,
        autoAlpha: 0,
      }, 0.78)
      .to(scooterRef.current, {
        x: 0,
        autoAlpha: 1,
        duration: 0.4,
        ease: "power2.out",
      }, 0.82)
      .to(progressRef.current, {
        scaleX: 0,
        duration: 0.46,
        ease: "power2.inOut",
      }, 0.88)
      .to([checkRef.current, parcelRef.current], {
        autoAlpha: 0,
        duration: 0.2,
      }, 1.12);

    activeTlRef.current = tl;
    return tl;
  }, [clearActiveTimeline]);

  const runErrorTimeline = useCallback(() => {
    clearActiveTimeline();

    const tl = gsap.timeline();

    tl.to(rootRef.current, {
      x: -4,
      duration: 0.06,
      yoyo: true,
      repeat: 5,
      ease: "power1.inOut",
      }, 0)
      .to(progressRef.current, {
        scaleX: 0,
        duration: 0.35,
        ease: "power2.inOut",
      }, 0)
      .to(scooterRef.current, {
        x: 0,
        duration: 0.35,
        ease: "power2.inOut",
      }, 0)
      .to(rootRef.current, {
        x: 0,
        duration: 0.1,
      });

    activeTlRef.current = tl;
    return tl;
  }, [clearActiveTimeline]);

  const handlePointerMove = useCallback((event) => {
    if (phase === "loading") {
      return;
    }

    const root = rootRef.current;
    if (!root) {
      return;
    }

    const rect = root.getBoundingClientRect();
    const nx = ((event.clientX - rect.left) / rect.width - 0.5) * 2;
    const ny = ((event.clientY - rect.top) / rect.height - 0.5) * 2;

    gsap.to(sceneRef.current, {
      x: nx * 6,
      y: ny * 2.5,
      duration: 0.2,
      ease: "power2.out",
      overwrite: true,
    });

    gsap.to(scooterRef.current, {
      rotate: nx * 7,
      duration: 0.25,
      ease: "power2.out",
      overwrite: true,
    });
  }, [phase]);

  const handlePointerLeave = useCallback(() => {
    if (phase === "loading") {
      return;
    }

    gsap.to(sceneRef.current, {
      x: 0,
      y: 0,
      duration: 0.35,
      ease: "power3.out",
      overwrite: true,
    });

    gsap.to(scooterRef.current, {
      rotate: 0,
      duration: 0.35,
      ease: "power3.out",
      overwrite: true,
    });
  }, [phase]);

  const handleClick = useCallback(async (event) => {
    if (isBusy) {
      return;
    }

    animateRipple(event);
    setPhase("loading");

    const dispatchTl = runDispatchTimeline();
    const actionPromise = Promise.resolve(onAction ? onAction() : null);

    let actionError = null;

    await Promise.all([
      actionPromise.catch((error) => {
        actionError = error;
      }),
      timelineDone(dispatchTl),
    ]);

    if (!mountedRef.current) {
      return;
    }

    if (actionError) {
      setPhase("error");
      await timelineDone(runErrorTimeline());
      if (!mountedRef.current) {
        return;
      }
      await wait(650);
      if (mountedRef.current) {
        resetScene();
        resumeIdle();
        setPhase("idle");
      }
      return;
    }

    setPhase("success");
    await timelineDone(runSuccessTimeline());
    if (!mountedRef.current) {
      return;
    }
    await wait(350);
    if (mountedRef.current) {
      resetScene();
      resumeIdle();
      setPhase("idle");
    }
  }, [
    animateRipple,
    isBusy,
    onAction,
    resetScene,
    resumeIdle,
    runDispatchTimeline,
    runErrorTimeline,
    runSuccessTimeline,
  ]);

  return (
    <button
      ref={rootRef}
      type="button"
      className={`shipment-btn btn-invert ${className}`}
      disabled={isBusy}
      onClick={handleClick}
      onPointerMove={handlePointerMove}
      onPointerLeave={handlePointerLeave}
      aria-live="polite"
    >
      <span ref={rippleRef} className="shipment-btn__ripple" />

      <svg
        ref={sceneRef}
        className="shipment-btn__scene"
        viewBox="0 0 200 54"
        xmlns="http://www.w3.org/2000/svg"
        aria-hidden="true"
      >
        <circle cx="22" cy="36" r="5.8" fill="rgba(12,12,12,0.83)" />
        <circle cx="22" cy="36" r="2.5" fill="rgba(255,255,255,0.9)" />

        <path
          ref={routeRef}
          d="M22 36 H178"
          stroke="rgba(12,12,12,0.54)"
          strokeWidth="2"
          strokeLinecap="round"
          strokeDasharray="4 5"
        />
        <path
          ref={progressRef}
          d="M22 36 H178"
          stroke="rgba(12,12,12,0.82)"
          strokeWidth="2.7"
          strokeLinecap="round"
        />

        <g ref={pinRef} transform="translate(178 34)">
          <path
            d="M0 -8c-3 0-5.2 2.28-5.2 5.04 0 3.44 5.2 8.82 5.2 8.82S5.2.48 5.2-2.96C5.2-5.72 3-8 0-8z"
            fill="rgba(8,8,8,0.85)"
          />
          <circle cx="0" cy="-2.8" r="1.8" fill="rgba(255,255,255,0.92)" />
        </g>

        <g ref={parcelRef} transform="translate(160 18)">
          <rect x="0" y="0" width="12" height="10" rx="2" fill="rgba(10,10,10,0.82)" />
          <path d="M6 0v10M0 4.8h12" stroke="rgba(255,255,255,0.72)" strokeWidth="1" />
        </g>

        <path
          ref={checkRef}
          d="M162 26l4 4 8-9"
          fill="none"
          stroke="rgba(8,8,8,0.92)"
          strokeWidth="2.4"
          strokeLinecap="round"
          strokeLinejoin="round"
        />

        <g ref={scooterRef}>
          <circle ref={wheelRearRef} cx="38" cy="36" r="4.6" fill="rgba(10,10,10,0.9)" />
          <circle ref={wheelFrontRef} cx="58" cy="36" r="4.6" fill="rgba(10,10,10,0.9)" />

          <rect x="41" y="24.8" width="12" height="7.2" rx="1.8" fill="rgba(13,13,13,0.88)" />
          <rect x="33" y="22.3" width="9.2" height="6.1" rx="1.6" fill="rgba(20,20,20,0.85)" />
          <circle cx="35.6" cy="20.9" r="2.2" fill="rgba(18,18,18,0.85)" />
          <rect x="52.7" y="24.5" width="7.8" height="1.9" rx="0.95" fill="rgba(24,24,24,0.85)" />
          <path d="M44 31.8h11.6" stroke="rgba(255,255,255,0.45)" strokeWidth="1.2" />
        </g>
      </svg>

      <span className={`shipment-btn__label shipment-btn__label--${phase}`}>
        {label}
      </span>
    </button>
  );
}
